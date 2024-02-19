/* Author: Steffen Viken Valvaag <steffenv@cs.uit.no> */
#include "common.h"
#include "list.h"
#include "printing.h"
#include "set.h"
#include <time.h>

/*
 * Case-insensitive comparison function for strings.
 */
static int compare_words(void *a, void *b)
{
    return strcasecmp(a, b);
}

/*
 * Returns the set of (unique) words found in the given file.
 */
static set_t *tokenize(char *filename)
{
    set_t *wordset = set_create(compare_words);
    list_t *wordlist = list_create(compare_words);
    list_iter_t *it;
    FILE *f;

    f = fopen(filename, "r");
    if (f == NULL)
    {
        perror("fopen");
        ERROR_PRINT("fopen() failed");
    }
    tokenize_file(f, wordlist);

    it = list_createiter(wordlist);
    while (list_hasnext(it))
    {
        set_add(wordset, list_next(it));
    }
    list_destroyiter(it);
    list_destroy(wordlist);
    return wordset;
}

/*
 * Prints a set of words.

static void printwords(char *prefix, set_t *words)
{
    set_iter_t *it;

    it = set_createiter(words);
    INFO_PRINT("%s: ", prefix);
    while (set_hasnext(it))
    {
        INFO_PRINT(" %s", (char *)set_next(it));
    }
    printf("\n");
    set_destroyiter(it);
}
*/

/*
 * Main entry point.
 */
int main(int argc, char **argv)
{
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    char *spamdir, *nonspamdir, *maildir;

    if (argc != 4)
    {
        DEBUG_PRINT("usage: %s <spamdir> <nonspamdir> <maildir>\n", argv[0]);
        return 1;
    }

    spamdir = argv[1];
    nonspamdir = argv[2];
    maildir = argv[3];

    list_t *spam_files = find_files(spamdir);
    list_t *nonspam_files = find_files(nonspamdir);
    list_t *mail_files = find_files(maildir);

    list_iter_t *it = list_createiter(spam_files);
    set_t *spamwords = set_create(compare_words);
    set_t *tmp = set_create(compare_words);

    while (list_hasnext(it)) {
        tmp = tokenize(list_next(it));
        if (!set_size(spamwords)) {
            spamwords = tmp;
        }
        spamwords = set_intersection(spamwords, tmp);
    }

    printf("Words contained in all spam mails %d\n", set_size(spamwords));

    it = list_createiter(nonspam_files);
    set_t *nonspamwords = set_create(compare_words);

    while (list_hasnext(it)) {
        tmp = tokenize(list_next(it));
        if (!set_size(nonspamwords)) {
            nonspamwords = tmp;
        }
        nonspamwords = set_union(nonspamwords, tmp);
    }
    printf("Unique words in non spam mails %d\n", set_size(nonspamwords));

    set_t *refined_spamword = set_difference(spamwords, nonspamwords);
    printf("Words contained in all spam mails and in none of the nonspam mails %d\n", set_size(refined_spamword));

    it = list_createiter(mail_files);
    set_t *check_mail = set_create(compare_words);

    while (list_hasnext(it)) {
        tmp = tokenize(list_next(it));
        check_mail = set_intersection(tmp, refined_spamword);
        if (set_size(check_mail)) {
            printf("Mail is spam! Mail contained %d spamwords\n", set_size(check_mail));
        }     
    }
     clock_gettime(CLOCK_MONOTONIC, &end_time);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                          (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Elapsed time: %.9f seconds\n", elapsed_time);
    return 0;
} 