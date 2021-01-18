## Utilities for unicode in Flex

### charclass

Outputs a regex to match UTF-8 byte sequences for all codepoints matching an
[ICU unicode
regex](https://unicode-org.github.io/icu/userguide/strings/regexp.html#regular-expression-metacharacters).

```sh
# all Chinese characters
./charclass '\p{Han}'

# horizontal whitespace
./charclass '\h'
```

The `\p` option is especially powerful because it can match [unicode
properties](https://en.wikipedia.org/wiki/Unicode_character_property#General_Category).

To use the regexes, give them aliases in your Flex file:

```lex
/* from charcode '\h' */
whitespace \x09|\x20|\xc2\xa0|\xe1\x9a\x80|\xe2\x80[\x80-\x8a]|\xe2\x80\xaf|\xe2\x81\x9f

%%

{whitespace}  { /* ... */ }
```

### Installation

Requires C99, [ICU](http://site.icu-project.org/download/), and
[pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/).

```sh
./configure
make
```
