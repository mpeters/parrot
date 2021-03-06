/*
Copyright (C) 2001-2010, Parrot Foundation.

=head1 NAME

src/string/encoding/ucs2.c - UCS-2 encoding

=head1 DESCRIPTION

UCS-2 encoding

=head2 Functions

=over 4

=cut

*/

#include "parrot/parrot.h"
#include "shared.h"

/* HEADERIZER HFILE: none */

/* HEADERIZER BEGIN: static */
/* Don't modify between HEADERIZER BEGIN / HEADERIZER END.  Your changes will be lost. */

static size_t ucs2_hash(SHIM_INTERP,
    ARGIN(const STRING *src),
    size_t hashval)
        __attribute__nonnull__(2);

static UINTVAL ucs2_iter_get(SHIM_INTERP,
    ARGIN(const STRING *str),
    ARGIN(const String_iter *i),
    INTVAL offset)
        __attribute__nonnull__(2)
        __attribute__nonnull__(3);

static UINTVAL ucs2_iter_get_and_advance(SHIM_INTERP,
    ARGIN(const STRING *str),
    ARGMOD(String_iter *i))
        __attribute__nonnull__(2)
        __attribute__nonnull__(3)
        FUNC_MODIFIES(*i);

static void ucs2_iter_set_and_advance(SHIM_INTERP,
    ARGMOD(STRING *str),
    ARGMOD(String_iter *i),
    UINTVAL c)
        __attribute__nonnull__(2)
        __attribute__nonnull__(3)
        FUNC_MODIFIES(*str)
        FUNC_MODIFIES(*i);

static void ucs2_iter_set_position(SHIM_INTERP,
    SHIM(const STRING *str),
    ARGMOD(String_iter *i),
    UINTVAL n)
        __attribute__nonnull__(3)
        FUNC_MODIFIES(*i);

static void ucs2_iter_skip(SHIM_INTERP,
    SHIM(const STRING *str),
    ARGMOD(String_iter *i),
    INTVAL skip)
        __attribute__nonnull__(3)
        FUNC_MODIFIES(*i);

static UINTVAL ucs2_ord(PARROT_INTERP, ARGIN(const STRING *src), INTVAL idx)
        __attribute__nonnull__(1)
        __attribute__nonnull__(2);

PARROT_WARN_UNUSED_RESULT
static UINTVAL ucs2_scan(PARROT_INTERP, ARGIN(const STRING *src))
        __attribute__nonnull__(1)
        __attribute__nonnull__(2);

PARROT_WARN_UNUSED_RESULT
PARROT_CANNOT_RETURN_NULL
static STRING * ucs2_to_encoding(PARROT_INTERP, ARGIN(const STRING *src))
        __attribute__nonnull__(1)
        __attribute__nonnull__(2);

#define ASSERT_ARGS_ucs2_hash __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(src))
#define ASSERT_ARGS_ucs2_iter_get __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(str) \
    , PARROT_ASSERT_ARG(i))
#define ASSERT_ARGS_ucs2_iter_get_and_advance __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(str) \
    , PARROT_ASSERT_ARG(i))
#define ASSERT_ARGS_ucs2_iter_set_and_advance __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(str) \
    , PARROT_ASSERT_ARG(i))
#define ASSERT_ARGS_ucs2_iter_set_position __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(i))
#define ASSERT_ARGS_ucs2_iter_skip __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(i))
#define ASSERT_ARGS_ucs2_ord __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(interp) \
    , PARROT_ASSERT_ARG(src))
#define ASSERT_ARGS_ucs2_scan __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(interp) \
    , PARROT_ASSERT_ARG(src))
#define ASSERT_ARGS_ucs2_to_encoding __attribute__unused__ int _ASSERT_ARGS_CHECK = (\
       PARROT_ASSERT_ARG(interp) \
    , PARROT_ASSERT_ARG(src))
/* Don't modify between HEADERIZER BEGIN / HEADERIZER END.  Your changes will be lost. */
/* HEADERIZER END: static */

#define UNIMPL Parrot_ex_throw_from_c_args(interp, NULL, EXCEPTION_UNIMPLEMENTED, \
    "unimpl ucs2")

/*

=item C<static STRING * ucs2_to_encoding(PARROT_INTERP, const STRING *src)>

Converts the string C<src> to this particular encoding.  If C<dest> is
provided, it will contain the result.  Otherwise this function operates in
place.

=cut

*/

PARROT_WARN_UNUSED_RESULT
PARROT_CANNOT_RETURN_NULL
static STRING *
ucs2_to_encoding(PARROT_INTERP, ARGIN(const STRING *src))
{
    ASSERT_ARGS(ucs2_to_encoding)
    STRING * const result =
        Parrot_utf16_encoding_ptr->to_encoding(interp, src);

    /* conversion to utf16 downgrades to ucs-2 if possible - check result */
    if (result->encoding == Parrot_utf16_encoding_ptr)
        Parrot_ex_throw_from_c_args(interp, NULL, EXCEPTION_INVALID_ENCODING,
            "can't convert string with surrogates to ucs2");

    return result;
}

/*

=item C<static UINTVAL ucs2_scan(PARROT_INTERP, const STRING *src)>

Returns the number of codepoints in string C<src>.

=cut

*/

PARROT_WARN_UNUSED_RESULT
static UINTVAL
ucs2_scan(PARROT_INTERP, ARGIN(const STRING *src))
{
    ASSERT_ARGS(ucs2_scan)
    return src->bufused >> 1;
}

/*

=item C<static UINTVAL ucs2_ord(PARROT_INTERP, const STRING *src, INTVAL idx)>

Returns the codepoint in string C<src> at position C<offset>.

=cut

*/

static UINTVAL
ucs2_ord(PARROT_INTERP, ARGIN(const STRING *src), INTVAL idx)
{
    ASSERT_ARGS(ucs2_ord)
    const UINTVAL len = STRING_length(src);
    const Parrot_UInt2 *s;

    if (idx < 0)
        idx += len;

    if ((UINTVAL)idx >= len)
        encoding_ord_error(interp, src, idx);

    s = (const Parrot_UInt2 *)src->strstart;

    return s[idx];
}


/*

=item C<static UINTVAL ucs2_iter_get(PARROT_INTERP, const STRING *str, const
String_iter *i, INTVAL offset)>

Get the character at C<i> + C<offset>.

=cut

*/

static UINTVAL
ucs2_iter_get(SHIM_INTERP,
    ARGIN(const STRING *str), ARGIN(const String_iter *i), INTVAL offset)
{
    ASSERT_ARGS(ucs2_iter_get)
    const Parrot_UInt2 * const s = (const Parrot_UInt2 *)str->strstart;

    return s[i->charpos + offset];
}

/*

=item C<static void ucs2_iter_skip(PARROT_INTERP, const STRING *str, String_iter
*i, INTVAL skip)>

Moves the string iterator C<i> by C<skip> characters.

=cut

*/

static void
ucs2_iter_skip(SHIM_INTERP,
    SHIM(const STRING *str), ARGMOD(String_iter *i), INTVAL skip)
{
    ASSERT_ARGS(ucs2_iter_skip)

    i->charpos += skip;
    i->bytepos += skip * 2;
}

/*

=item C<static UINTVAL ucs2_iter_get_and_advance(PARROT_INTERP, const STRING
*str, String_iter *i)>

Moves the string iterator C<i> to the next UCS-2 codepoint.

=cut

*/

static UINTVAL
ucs2_iter_get_and_advance(SHIM_INTERP,
    ARGIN(const STRING *str), ARGMOD(String_iter *i))
{
    ASSERT_ARGS(ucs2_iter_get_and_advance)
    const Parrot_UInt2 * const s = (Parrot_UInt2 *)str->strstart;
    const UINTVAL c = s[i->charpos];

    i->charpos++;
    i->bytepos += 2;

    return c;
}

/*

=item C<static void ucs2_iter_set_and_advance(PARROT_INTERP, STRING *str,
String_iter *i, UINTVAL c)>

With the string iterator C<i>, appends the codepoint C<c> and advances to the
next position in the string.

=cut

*/

static void
ucs2_iter_set_and_advance(SHIM_INTERP,
    ARGMOD(STRING *str), ARGMOD(String_iter *i), UINTVAL c)
{
    ASSERT_ARGS(ucs2_iter_set_and_advance)
    Parrot_UInt2 * const s = (Parrot_UInt2 *) str->strstart;

    s[i->charpos] = c;

    i->charpos++;
    i->bytepos += 2;
}

/*

=item C<static void ucs2_iter_set_position(PARROT_INTERP, const STRING *str,
String_iter *i, UINTVAL n)>

Moves the string iterator C<i> to the position C<n> in the string.

=cut

*/

static void
ucs2_iter_set_position(SHIM_INTERP,
    SHIM(const STRING *str), ARGMOD(String_iter *i), UINTVAL n)
{
    ASSERT_ARGS(ucs2_iter_set_position)

    i->charpos = n;
    i->bytepos = n * 2;
}

/*

=item C<static size_t ucs2_hash(PARROT_INTERP, const STRING *src, size_t
hashval)>

Returns the hashed value of the string, given a seed in hashval.

=cut

*/

static size_t
ucs2_hash(SHIM_INTERP, ARGIN(const STRING *src), size_t hashval)
{
    ASSERT_ARGS(ucs2_hash)
    DECL_CONST_CAST;
    STRING * const s = PARROT_const_cast(STRING *, src);
    const Parrot_UInt2 *pos;
    UINTVAL len;

    pos = (const Parrot_UInt2*)s->strstart;
    len = s->strlen;

    while (len--) {
        hashval += hashval << 5;
        hashval += *(pos++);
    }

    s->hashval = hashval;

    return hashval;
}

static STR_VTABLE Parrot_ucs2_encoding = {
    0,
    "ucs2",
    NULL,
    2, /* Max bytes per codepoint */

    ucs2_to_encoding,
    unicode_chr,

    encoding_equal,
    encoding_compare,
    encoding_index,
    encoding_rindex,
    encoding_hash,
    unicode_validate,

    ucs2_scan,
    ucs2_ord,
    fixed_substr,

    encoding_is_cclass,
    encoding_find_cclass,
    encoding_find_not_cclass,

    encoding_get_graphemes,
    unicode_compose,
    encoding_decompose,

    unicode_upcase,
    unicode_downcase,
    unicode_titlecase,
    unicode_upcase_first,
    unicode_downcase_first,
    unicode_titlecase_first,

    ucs2_iter_get,
    ucs2_iter_skip,
    ucs2_iter_get_and_advance,
    ucs2_iter_set_and_advance,
    ucs2_iter_set_position
};

STR_VTABLE *Parrot_ucs2_encoding_ptr = &Parrot_ucs2_encoding;


/*

=back

=head1 SEE ALSO

F<src/string/encoding/fixed_8.c>,
F<src/string/encoding/utf8.c>,
F<src/string.c>,
F<include/parrot/string.h>,
F<docs/string.pod>.

=cut

*/


/*
 * Local variables:
 *   c-file-style: "parrot"
 * End:
 * vim: expandtab shiftwidth=4:
 */
