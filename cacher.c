#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct item item_t;
typedef struct cache cache_t;

cache_t *create_cache(int size, char *flag);
void clean_cache(cache_t **c);
int find_item(cache_t *c, char *item_name);
int check_if_item_dropped_before(cache_t *c, char *item_name);
void append_item_to_cache(cache_t *c, item_t *new_item);
void drop_item_from_cache(cache_t *c, item_t *item_to_drop);
void process_item_to_cache(cache_t *c, item_t *new_item);
void store_item(cache_t *c, char *item_name);

struct item {
    char *item_name;
    int referenced;
    item_t *next;
};

struct cache {
    item_t *head;
    item_t *dropped_items;
    int size;
    int counter;
    int compulsory_misses;
    int capacity_misses;
    bool first_in_first_out;
    bool least_recently_used;
    bool clock;
};

cache_t *create_cache(int size, char *flag) {
    cache_t *c = (cache_t *) calloc(1, sizeof(cache_t));
    c->head = NULL;
    c->dropped_items = NULL;
    c->size = size;
    c->counter = 0;
    c->compulsory_misses = 0;
    c->capacity_misses = 0;
    if (strcmp(flag, "F") == 0) {
        c->first_in_first_out = true;
        c->least_recently_used = false;
        c->clock = false;
    } else if (strcmp(flag, "L") == 0) {
        c->first_in_first_out = false;
        c->least_recently_used = true;
        c->clock = false;
    } else {
        c->first_in_first_out = false;
        c->least_recently_used = false;
        c->clock = true;
    }
    return c;
}

void clean_cache(cache_t **c) {
    item_t *here = (*c)->head;
    while (here != NULL) {
        free(here->item_name);
        item_t *freethis = here;
        here = here->next;
        free(freethis);
    }
    here = (*c)->dropped_items;
    while (here != NULL) {
        free(here->item_name);
        item_t *freethis = here;
        here = here->next;
        free(freethis);
    }
    free(*c);
    *c = NULL;
}

int find_item(cache_t *c, char *item_name) {
    item_t *here = c->head;
    if (c->first_in_first_out) {
        while (here != NULL) {
            if (strcmp(here->item_name, item_name) == 0) {
                return 1;
            }
            here = here->next;
        }
    } else if (c->least_recently_used) {
        if (here == NULL) {
            return 0;
        }
        if (strcmp(c->head->item_name, item_name) == 0) {
            item_t *changethis = c->head;
            c->head = changethis->next;
            changethis->next = NULL;
            append_item_to_cache(c, changethis);
            return 1;
        } else {
            while (here->next != NULL) {
                if (strcmp(here->next->item_name, item_name) == 0) {
                    item_t *changethis = here->next;
                    here->next = changethis->next;
                    changethis->next = NULL;
                    append_item_to_cache(c, changethis);
                    return 1;
                }
                here = here->next;
            }
        }
    } else {
        while (here != NULL) {
            if (strcmp(here->item_name, item_name) == 0) {
                here->referenced = 1;
                return 1;
            }
            here = here->next;
        }
    }
    return 0;
}

int check_if_item_dropped_before(cache_t *c, char *item_name) {
    item_t *here = c->dropped_items;
    while (here != NULL) {
        if (strcmp(here->item_name, item_name) == 0) {
            return 1;
        }
        here = here->next;
    }
    return 0;
}

void append_item_to_cache(cache_t *c, item_t *new_item) {
    if (c->head == NULL) {
        c->head = new_item;
    } else {
        item_t *here = c->head;
        while (here->next != NULL) {
            here = here->next;
        }
        here->next = new_item;
    }
}

void drop_item_from_cache(cache_t *c, item_t *item_to_drop) {
    if (c->dropped_items == NULL) {
        c->dropped_items = item_to_drop;
    } else {
        item_t *here = c->dropped_items;
        item_t *prev;
        bool dropped_more_than_once = false;
        while (here != NULL) {
            if (strcmp(here->item_name, item_to_drop->item_name) == 0) {
                dropped_more_than_once = true;
                break;
            }
            prev = here;
            here = here->next;
        }
        if (dropped_more_than_once) {
            free(item_to_drop->item_name);
            free(item_to_drop);
        } else {
            prev->next = item_to_drop;
        }
    }
}

void process_item_to_cache(cache_t *c, item_t *new_item) {
    if (c->counter < c->size) {
        c->counter++;
    } else {
        if (c->first_in_first_out || c->least_recently_used) {
            item_t *removethis = c->head;
            c->head = removethis->next;
            removethis->next = NULL;
            drop_item_from_cache(c, removethis);
        } else {
            bool cache_avaliable = false;
            item_t *here = c->head;
            item_t *prev;
            while (here != NULL) {
                if (cache_avaliable) {
                    break;
                } else if (here == c->head) {
                    if (c->head->referenced == 0) {
                        item_t *removethis = c->head;
                        c->head = removethis->next;
                        removethis->next = NULL;
                        drop_item_from_cache(c, removethis);
                        cache_avaliable = true;
                    } else {
                        item_t *changethis = c->head;
                        c->head = changethis->next;
                        changethis->referenced = 0;
                        changethis->next = NULL;
                        append_item_to_cache(c, changethis);
                    }
                } else {
                    if (here->referenced == 0) {
                        item_t *removethis = here;
                        prev->next = removethis->next;
                        removethis->next = NULL;
                        drop_item_from_cache(c, removethis);
                        cache_avaliable = true;
                    } else {
                        item_t *changethis = here;
                        prev->next = changethis->next;
                        changethis->referenced = 0;
                        changethis->next = NULL;
                        append_item_to_cache(c, changethis);
                    }
                }
                prev = here;
                here = here->next;
                if (here == NULL) {
                    here = c->head;
                }
            }
        }
    }
    append_item_to_cache(c, new_item);
}

void store_item(cache_t *c, char *item_name) {
    if (check_if_item_dropped_before(c, item_name) == 0) {
        c->compulsory_misses++;
    } else {
        c->capacity_misses++;
    }
    item_t *new_item = (item_t *) calloc(1, sizeof(item_t));
    new_item->item_name = strdup(item_name);
    if (c->clock) {
        new_item->referenced = 0;
    }
    new_item->next = NULL;
    process_item_to_cache(c, new_item);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s [-N size]\n", argv[0]);
        exit(1);
    }
    int opt_val;
    int cache_size = -1;
    char *flag = "F";
    char *endptr = NULL;
    char *buf = NULL;
    size_t len = 0;
    cache_t *c;
    while ((opt_val = getopt(argc, argv, "N:FLC")) != -1) {
        if (opt_val == 'N') {
            cache_size = (int) strtoull(optarg, &endptr, 10);
        } else if (opt_val == 'L') {
            flag = "L";
        } else if (opt_val == 'C') {
            flag = "C";
        }
    }
    if (cache_size <= 0) {
        fprintf(stderr, "invalid cache size argument\n");
        exit(1);
    }
    c = create_cache(cache_size, flag);
    while (getline(&buf, &len, stdin) > 0) {
        char *tok = strtok(buf, "\n");
        if (find_item(c, tok) == 1) {
            printf("HIT\n");
        } else {
            store_item(c, tok);
            printf("MISS\n");
        }
    }
    printf("%d %d\n", c->compulsory_misses, c->capacity_misses);
    free(buf);
    clean_cache(&c);
    return 0;
}
