#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <unicode/uregex.h>
#include <unicode/ustring.h>
#include <unicode/utf8.h>


/*
 * ([a-c]|[d-f]|...|[x-z])(...)(...)
 */

#define CODEPOINT_MAX 0xF0000
#define UTF8_LEN 4

bool bytematch[UTF8_LEN][UCHAR_MAX];

int main(int argc, char **argv)
{
	UChar32 s[2] = {0};
	URegularExpression *r;
	UErrorCode status;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s regex\n", *argv);
		return EXIT_FAILURE;
	}

	r = uregex_openC(argv[1], 0, NULL, &status);
	if (U_FAILURE(status))
	{
		fputs("Failed to parse regex\n", stderr);
		return EXIT_FAILURE;
	}

	for (s[0] = 1; s[0] < CODEPOINT_MAX; s[0]++)
	{
		unsigned char bytes[UTF8_LEN] = {0};
		UChar haystack[UTF8_LEN] = {0};
		size_t i, written = 0;
		UBool err = FALSE;

		u_strFromUTF32(haystack, UTF8_LEN, NULL, s, -1, &status);
		if (U_FAILURE(status))
		{
			fprintf(stderr, "Couldn't convert to utf-16: %ld\n", (long)s[0]);
			continue;
		}
		uregex_setText(r, haystack, -1, &status);
		if (uregex_matches(r, -1, &status) == FALSE /* ICU bool */)
			continue;

		U8_APPEND(bytes, written, UTF8_LEN, s[0], err);
		if (err)
		{
			fprintf(stderr, "Skipping invalid character: %ld\n", (long)s[0]);
			continue;
		}
		for (i = 0; i < written; i++)
			bytematch[i][bytes[i]] = true;
	}
	uregex_close(r);
	return EXIT_SUCCESS;
}
