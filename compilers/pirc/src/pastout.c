/*

=head1 NAME

pastout.c - routines for emitting PAST

=head1 STATUS

Initial.

=cut

*/

#include "pastout.h"
#include "pirvtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/* keep outputfile possibility easy */
#define OUT         stderr
#define INDENT      4
#define indent(D)   D->indent += INDENT
#define dedent(D)   D->indent -= INDENT


/*

=head1 DATA STRUCTURE

The PAST back-end implements the C<emit_data> data structure.
Currently, only a single data member is used, to control the
indention.

=cut

*/
typedef struct emit_data {
    int indent;

} emit_data;


/*

=head1 PAST VTABLE ENTRIES

=over 4

=item past_init()

Prints initializing PAST: ["past" => PMC 'PAST::Block' {]

=cut

*/
static void
past_init(struct emit_data *data) {
    fprintf(OUT, "\"past\" => PMC 'PAST::Block'  {\n");
    indent(data);
}

/*

=item past_block()

Prints PAST for a block, including <source> and <pos> attributes.

=cut

*/
static void
past_block(struct emit_data *data, char *source, int pos) {
    fprintf(OUT, "%*sPMC 'PAST::Block'  {\n", data->indent, " ");
    indent(data);
    fprintf(OUT, "%*s<source> => \"%s\"\n", data->indent, " ", source);
    fprintf(OUT, "%*s<pos> => %d\n", data->indent, " ", pos);
}

/*

=item past_close()

Close down a PAST entity. The closing brace is printed, and the
indention level is decremented.

=cut

*/
static void
past_close(struct emit_data *data) {
    dedent(data);
    /* check for indent == 0, if so, then print no space, this is
     * so otherwise a single space is printed.
     */
    fprintf(OUT, "%*s}\n", data->indent, (data->indent == 0) ? "" : " ");

}

/*

=item past_name()

Prints a PAST <name> entry.

=cut

*/
static void
past_name(struct emit_data *data, char *name) {
    fprintf(OUT, "%*s<name> => \"%s\"\n", data->indent, " ", name);
}

/*

=item past_stmts()

Opens a PAST::Stmts node.

=cut

*/
static void
past_stmts(struct emit_data *data) {
    fprintf(OUT, "%*s[%d] => PMC 'PAST::Stmts'  {\n", data->indent, " ", 0); /* fix array index */
    indent(data);
}

/*

=item past_param()

Generates a PAST::Var node and set its scope attribute to "parameter".

=cut

*/
static void
past_param(struct emit_data *data) {
    fprintf(OUT, "%*sFIXTHIS => PMC 'PAST::Var'  {\n", data->indent, " ");
    indent(data);
    fprintf(OUT, "%*s<scope> => \"parameter\"\n", data->indent, " ");
}

/*

=item past_type()



=cut

*/
static void
past_type(struct emit_data *data, char *type) {
    fprintf(OUT, "%*s<type> => \"%s\"\n", data->indent, " ", type);
}

/*

=item past_subflag()



=cut

*/
static void
past_subflag(struct emit_data *data, int flag) {
    /* fprintf(OUT, "%*s<???> => \"%s\"\n", data->indent, " ", type);
    */
}


/*

=item past_op()



=cut

*/
static void
past_op(struct emit_data *data, char *op) {
    fprintf(OUT, "%*sFIXME => PMC 'PAST::Op'  {\n", data->indent, " ");
    indent(data);
    fprintf(OUT, "%*s<pirop> => \"%s\"\n", data->indent, " ", op);

}

/*

=item past_expr()



=cut

*/
static void
past_expr(struct emit_data *data, char *expr) {
    fprintf(OUT, "%*s[%d] => \"%s\"\n", data->indent, " ", 0, expr); /* fix index */
}

/*

=item past_destroy()



=cut

*/
static void
past_destroy(emit_data *data) {
    free(data);
    data = NULL;
}
/*

=item init_past_vtable()

Creates a vtable for the PAST emitting module, and
then this vtable is set into the parser_state struct.

=cut

*/
pirvtable *
init_past_vtable(void) {
    pirvtable *vtable = new_pirvtable();

    /* override vtable methods */
    vtable->initialize   = past_init;
    vtable->destroy      = past_destroy;
    vtable->sub_start    = past_block;
    vtable->sub_end      = past_close;
    vtable->name         = past_name;
    vtable->stmts_start  = past_stmts;
    vtable->stmts_end    = past_close;
    vtable->end          = past_close;
    vtable->param_start  = past_param;
    vtable->param_end    = past_close;
    vtable->type         = past_type;
    vtable->sub_flag     = past_subflag;
    vtable->op_start     = past_op;
    vtable->expression   = past_expr;
    vtable->op_end       = past_close;

    vtable->data = (emit_data *)malloc(sizeof(emit_data));
    if (vtable->data == NULL) {
        fprintf(stderr, "Failed to allocate memory for vtable data\n");
        exit(1);
    }
    vtable->data->indent = 0;

    return vtable;
}



/*

=back

=cut

*/


/*
 * Local variables:
 *   c-file-style: "parrot"
 * End:
 * vim: expandtab shiftwidth=4:
 */
