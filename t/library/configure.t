#!parrot
# Copyright (C) 2009, Parrot Foundation.
# $Id$

=head1 NAME

t/library/configure.t - Test the Configure PBC

=head1 SYNOPSIS

    % prove t/library/configure.t

=cut

.sub 'main' :main
    .include 'test_more.pir'

    load_bytecode 'Configure.pbc'

    plan(23)
    test_conditioned_line()
    test_interpolate_var()
    test_replace_slash()
.end

.sub 'test_conditioned_line'
    .local pmc config
    config = new 'Hash'
    config['foo'] = 1
    config['bar'] = 0
    config['baz'] = 1

    $S0 = conditioned_line("plain text\nwithout #", config)
    is($S0, "plain text\nwithout #\n", "plain text")

    $S0 = conditioned_line("#IF(malformed", config)
    is($S0, "#IF(malformed\n", "malformed")

    $S0 = conditioned_line("#IF(foo):positive", config)
    is($S0, "positive\n", "#IF positive")
    $S0 = conditioned_line("#IF(bar):negative", config)
    is($S0, "", "#IF negative")

    $S0 = conditioned_line("#UNLESS(bar):positive", config)
    is($S0, "positive\n", "#UNLESS positive")
    $S0 = conditioned_line("#UNLESS(foo):negative", config)
    is($S0, "", "#UNLESS negative")

    $S0 = conditioned_line("#IF(foo):positive\n#ELSE:alternate", config)
    is($S0, "positive\n", "#IF/ELSE positive")
    $S0 = conditioned_line("#IF(bar):negative\n#ELSE:alternate", config)
    is($S0, "alternate\n", "#IF/ELSE alternate")

    $S0 = conditioned_line("#IF(foo):positive\n#ELSIF(baz):alternate", config)
    is($S0, "positive\n", "#IF/ELSIF positive")
    $S0 = conditioned_line("#IF(bar):negative\n#ELSIF(baz):alternate", config)
    is($S0, "alternate\n", "#IF/ELSIF alternate")
    $S0 = conditioned_line("#IF(bar):negative\n#ELSIF(bar):negative", config)
    is($S0, "", "#IF/ELSIF negative")
.end

.sub 'test_interpolate_var'
    .local pmc config
    config = new 'Hash'
    config['foo'] = 'bar'
    $S0 = interpolate_var("# plain text", config)
    is($S0, "# plain text\n", "plain text")
    $S0 = interpolate_var("\t@echo foo", config)
    is($S0, "\t@echo foo\n", "@ alone")
    $S0 = interpolate_var("here @foo@ variable", config)
    is($S0, "here bar variable\n", "variable")
    $S0 = interpolate_var("here @foo@ variable, and here @foo@.", config)
    is($S0, "here bar variable, and here bar.\n", "variable")
.end

.sub 'test_replace_slash'
    $S1 = "path/to/somewhere/"
    $S0 = replace_slash($S1, 'MSWin32')
    is($S0, "path\\to\\somewhere\\", "paths on win32")
    $S0 = replace_slash($S1, '*nix')
    is($S0, $S1, "paths on *nix")

    $S1 = "prove t/*.t"
    $S0 = replace_slash($S1, 'MSWin32')
    is($S0, "prove t\\\\*.t")
    $S0 = replace_slash($S1, '*nix')
    is($S0, $S1)

    $S1 = "prove t//*.t"
    $S0 = replace_slash($S1, 'MSWin32')
    is($S0, "prove t/*.t")
    $S0 = replace_slash($S1, '*nix')
    is($S0, "prove t/*.t")

    $S1 = "http:////host//paths//"
    $S0 = replace_slash($S1, 'MSWin32')
    is($S0, "http://host/paths/", "url on win32")
    $S0 = replace_slash($S1, '*nix')
    is($S0, "http://host/paths/", "url on *nix")
.end


# Local Variables:
#   mode: cperl
#   cperl-indent-level: 4
#   fill-column: 100
# End:
# vim: expandtab shiftwidth=4: