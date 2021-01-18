#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	UParseError pe;
	UErrorCode status = U_ZERO_ERROR;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s regex\n", *argv);
		return EXIT_FAILURE;
	}

	r = uregex_openC(argv[1], 0, &pe, &status);
	if (U_FAILURE(status))
	{
		fprintf(stderr, "Failed to parse regex: %s\n", argv[1]);
		fprintf(stderr, "Line %d, col %d\n", pe.line, pe.offset);
		return EXIT_FAILURE;
	}

	unsigned char bytes_first[UTF8_LEN] = {0},
	              bytes_prev[UTF8_LEN] = {0};
	bool first_streak = true;

	putchar('(');
	for (s[0] = 1; s[0] < CODEPOINT_MAX; s[0]++)
	{
		unsigned char bytes[UTF8_LEN] = {0};
		UChar haystack[UTF8_LEN] = {0};
		size_t written = 0;
		UBool err = FALSE;

		status = U_ZERO_ERROR;
		u_strFromUTF32(haystack, UTF8_LEN, NULL, s, -1, &status);
		if (U_FAILURE(status))
			continue;
		uregex_setText(r, haystack, -1, &status);
		if (uregex_matches(r, -1, &status) == FALSE /* ICU bool */)
			continue;

		U8_APPEND(bytes, written, UTF8_LEN, s[0], err);
		if (err)
		{
			fprintf(stderr, "Skipping invalid character: %ld|", (long)s[0]);
			continue;
		}
		if (memcmp(bytes_prev, bytes, written-1) != 0 ||
			bytes[written-1]-bytes_prev[written-1] != 1)
		{
			if (bytes_prev[0])
			{
				if (!first_streak)
					putchar('|');
				first_streak = false;
				for (size_t i = 0; i < written-1; i++)
					printf("\\x%02x", bytes_prev[i]);
				if (bytes_first[written-1] == bytes_prev[written-1])
					printf("\\x%02x", bytes_first[written-1]);
				else
					printf("[\\x%02x-\\x%02x]",
							bytes_first[written-1], bytes_prev[written-1]);
			}
			memcpy(bytes_first, bytes, sizeof bytes);
		}
		memcpy(bytes_prev, bytes, sizeof bytes);
	}
	puts(")");
	uregex_close(r);
	return EXIT_SUCCESS;
}
