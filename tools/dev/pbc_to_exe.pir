#! parrot
# Copyright (C) 2009-2010, Parrot Foundation.

=head1 NAME

pbc_to_exe - Generate executables from Parrot bytecode

=head1 DESCRIPTION

Compile bytecode to executable.

=head2 SYNOPSIS

  pbc_to_exe my.pbc
  => my.exe

  pbc_to_exe my.pbc --install
  => installable_my.exe

=cut

.include 'interpcores.pasm'

.sub 'main' :main
    .param pmc    argv

    load_bytecode 'config.pbc'

    .local string infile
    .local string cfile
    .local string objfile
    .local string exefile
    .local int    runcore
    .local int    install

    (infile, cfile, objfile, exefile, runcore, install) = 'handle_args'(argv)
    unless infile > '' goto err_infile

    .local string code_type
    code_type = 'determine_code_type'()

    .local string codestring
    if code_type == 'gcc'  goto code_for_gcc
    if code_type == 'msvc' goto code_for_msvc
    goto code_for_default
  code_for_gcc:
    codestring = 'generate_code_gcc'(infile)
    goto code_end
  code_for_msvc:
    codestring = 'generate_code_msvc'(infile)
    goto code_end
  code_for_default:
    codestring = 'generate_code'(infile)
  code_end:


  open_outfile:
    .local pmc outfh
    outfh = new ['FileHandle']
    outfh.'open'(cfile, 'w')
    unless outfh goto err_outfh
    print outfh, <<'HEADER'
#include "parrot/parrot.h"
#include "parrot/embed.h"
const void * get_program_code(void);
HEADER

    print outfh, codestring

    print outfh, '#define RUNCORE '
    print outfh, runcore
    print outfh, "\n"

    print outfh, <<'MAIN'
        int main(int argc, const char *argv[])
        {
            PackFile     *pf;
            Parrot_Interp interp;
            const unsigned char *program_code_addr;

            program_code_addr = (const unsigned char *)get_program_code();
            if (!program_code_addr)
                return 1;

            Parrot_set_config_hash();

            interp = Parrot_new( NULL );
            if (!interp)
                return 1;

            Parrot_init_stacktop(interp, &interp);
            Parrot_set_executable_name(interp,
                Parrot_str_new(interp, argv[0], 0));
            Parrot_set_run_core(interp, (Parrot_Run_core_t)RUNCORE);
            Parrot_set_flag(interp, PARROT_DESTROY_FLAG);

            pf = PackFile_new(interp, 0);
            if (!pf)
                return 1;

            if (!PackFile_unpack(interp, pf,
                    (const opcode_t *)program_code_addr, bytecode_size))
                return 1;

            do_sub_pragmas(interp, pf->cur_cs, PBC_PBC, NULL);

            Parrot_pbc_load(interp, pf);

            PackFile_fixup_subs(interp, PBC_MAIN, NULL);
            Parrot_runcode(interp, argc, argv);
            Parrot_destroy(interp);
            Parrot_exit(interp, 0);
        }
MAIN


    # The close opcode does not return a result code,
    # use the method instead.
    .local int    closeresult
    closeresult = outfh.'close'()
    unless closeresult == 0 goto err_close


    .local string extra_obj
    extra_obj = ''
    if code_type != 'msvc' goto no_extra
    extra_obj  = 'replace_pbc_extension'(infile, '.RES')
  no_extra:


    'compile_file'(cfile, objfile, install)
    'link_file'(objfile, exefile, extra_obj, install)
    .return ()

  err_infile:
    die "cannot read infile"
  err_outfh:
    die "cannot write outfile"
  err_close:
    die "cannot close outfile"
.end


.sub 'handle_args'
    .param pmc argv

    .local string obj, exe
    $P0    = '_config'()
    obj    = $P0['o']
    exe    = $P0['exe']

    load_bytecode 'Getopt/Obj.pbc'
    .local pmc getopt
    getopt = new ['Getopt';'Obj']
    push getopt, 'install|i'
    push getopt, 'runcore|R:s'
    push getopt, 'output|o:s'
    push getopt, 'help|h'

    $P0 = shift argv # ignore program name
    .local pmc opts
    opts = getopt.'get_options'(argv)

    .local int    help
    .local int    install
    .local string runcore
    .local string outfile
    help    = opts['help']
    install = opts['install']
    runcore = opts['runcore']
    outfile = opts['output']

    unless help goto end_help
        $P0 = getstderr
        print $P0, <<'HELP'
pbc_to_exe [options] <file>
  Options:
    -h --help
    -i --install
    -R --runcore=slow|fast
    -o --output=FILE
HELP
        exit 0
    end_help:

    .local string infile
    infile = shift argv

    $S0 = substr infile, -4, 4
    $S0 = downcase $S0
    unless $S0 != '.pbc' goto done_pbc_extn_check
        die "input pbc file name does not end in '.pbc'"
    done_pbc_extn_check:

    .local string cfile, objfile, exefile
    if outfile == '' goto no_outfile
        $I0 = length exe
        $I1 = - $I0
        $S0 = substr outfile, $I1, $I0
        $S0 = downcase $S0
        $S1 = downcase exe
        unless $S0 != $S1 goto done_exe_extn_check
            $S0 = "output executable name does not end in `" . exe
            $S0 = $S0 . "'"
            die $S0
        done_exe_extn_check:
        outfile = replace outfile, $I1, $I0, ''

        cfile   = outfile . '.c'
        objfile = outfile . obj
        exefile = outfile . exe
        goto end_outfile
    no_outfile:
        # substitute .c for .pbc
        # remove .c for executable
        outfile = replace infile, -4, 4, '' # remove .pbc extension
        cfile   = outfile . '.c'
        objfile = outfile . obj
        exefile = outfile . exe
        unless install goto end_installable
            exefile = 'prepend_installable'(exefile)
        end_installable:
    end_outfile:

    .local int runcore_code
    unless runcore == 'slow' goto end_slow_core
        runcore_code = .PARROT_SLOW_CORE
        goto done_runcore
    end_slow_core:
    unless runcore == 'fast' goto end_fast_core
        runcore_code = .PARROT_FAST_CORE
        goto done_runcore
    end_fast_core:
    unless runcore == '' goto end_unspecified_core
        runcore_code = .PARROT_FAST_CORE
        goto done_runcore
    end_unspecified_core:
        # invalid runcore name
        $S0 = "Unsupported runcore: `" . runcore
        $S0 = $S0 . "'"
        die $S0
    done_runcore:

    .return (infile, cfile, objfile, exefile, runcore_code, install)
.end

.sub 'determine_code_type'
    .local pmc    config
    .local string gcc_ver
    .local string cc
    .local string os_name

    config = '_config'()

    gcc_ver = config['gccversion']
    unless gcc_ver > '' goto not_gcc
    .return ('gcc')
  not_gcc:

    cc      = config['cc']
    os_name = config['osname']

    if os_name != 'MSWin32' goto not_msvc
    if cc      != 'cl'      goto not_msvc
    .return ('msvc')
  not_msvc:

    .return ('default')
.end


.sub 'generate_code'
    .param string infile
    .local pmc ifh
    ifh = new ['FileHandle']
    ifh.'open'(infile, 'r')
    unless ifh goto err_infile

    .local pmc codestring
    .local int size

    codestring = new [ 'StringBuilder' ]
    push codestring, "const Parrot_UInt1 program_code[] = {"
    size = 0

  read_loop:
    .local string pbcstring
    .local int pbclength

    pbcstring = ifh.'read'(16384)
    pbclength = length pbcstring
    unless pbclength > 0 goto read_done

    .local int pos
    pos = 0
  code_loop:
    unless pos < pbclength goto code_done
    $I0 = ord pbcstring, pos
    $S0 = $I0
    push codestring, $S0
    push codestring, ','
    inc pos
    inc size
    $I0 = size % 32
    unless $I0 == 0 goto code_loop
    push codestring, "\n"
    goto code_loop
  code_done:
    goto read_loop

  read_done:
    ifh.'close'()

    push codestring, "\n};\n\nconst size_t bytecode_size = "
    $S0 = size
    push codestring, $S0
    push codestring, ";\n"
    push codestring, <<'END_OF_FUNCTION'
        const void * get_program_code(void)
        {
            return program_code;
        }
END_OF_FUNCTION

    $S0 = codestring
    .return ($S0)

  err_infile:
    die "cannot open infile"
.end


# The PBC will be represented as a C string, so this sub builds a table
# of the C representation of each ASCII character, for lookup by ordinal value.
.sub 'generate_encoding_table'
    # Use '\%o' for speed, or '\x%02x' for readability
    .const string encoding_format = '\%o'

    # The 'sprintf' op requires the arglist to be in an array, even when
    # there is only one arg.
    .local pmc one_number
    one_number    = new 'FixedIntegerArray'
    set one_number, 1

    .local pmc coded_strings
    coded_strings = new 'FixedStringArray'
    set coded_strings, 256

    .local int index
    index = 0

  next_index:
    one_number[0] = index
    $S0 = sprintf encoding_format, one_number
    coded_strings[index] = $S0
    inc index
    if index < 256 goto next_index

    .return (coded_strings)
.end

.sub 'generate_code_gcc'
    .param string infile
    .local pmc ifh
    ifh = new ['FileHandle']
    ifh.'open'(infile, 'r')
    unless ifh goto err_infile

    .local pmc encoding_table
    encoding_table = 'generate_encoding_table'()

    .local pmc codestring
    .local int size

    codestring = new ['StringBuilder']

    push codestring, "const char * program_code =\n"
    push codestring, '"'
    size = 0

  read_loop:
    .local string pbcstring
    .local int pbclength

    pbcstring = ifh.'read'(16384)
    pbclength = length pbcstring
    unless pbclength > 0 goto read_done

    .local int pos
    pos = 0
  code_loop:
    unless pos < pbclength goto code_done
    $I0 = ord pbcstring, pos
    $S0 = encoding_table[$I0]
    push codestring, $S0
    inc pos
    inc size
    $I0 = size % 32
    unless $I0 == 0 goto code_loop
    push codestring, '"'
    push codestring, "\n"
    push codestring, '"'
    goto code_loop
  code_done:
    goto read_loop

  read_done:
    ifh.'close'()

    push codestring, '"'
    push codestring, "\n;\n\n"
    push codestring, "const size_t bytecode_size = "
    $S0 = size
    push codestring, $S0
    push codestring, ";\n"

    push codestring, <<'END_OF_FUNCTION'
        const void * get_program_code(void)
        {
            return program_code;
        }
END_OF_FUNCTION

    $S0 = codestring
    .return ($S0)

  err_infile:
    die "cannot open infile"
.end


# Transforms the .pbc path into one with a different extension.
# Passing '' means no extension.
# Extensions without leading dots will have a dot pre-pended.
.sub 'replace_pbc_extension'
    .param string pbc_path
    .param string new_extension

    $S0 = substr pbc_path, -4
    $S0 = downcase $S0
    if $S0 != '.pbc' goto err_pbc_path_not_pbc
    .local string base_path
     base_path = replace pbc_path, -4, 4, ''

    .local string new_path
    new_path = substr base_path, 0

    unless new_extension > '' goto ext_null

    $S1 = substr new_extension, 0, 1
    if $S1 == '.' goto has_dot
    new_path .= '.'

  has_dot:
    new_path .= new_extension

  ext_null:
    .return (new_path)

  err_pbc_path_not_pbc:
    die "input pbc file name does not end in '.pbc'"
.end


# In addition to generating the code for inclusion in the C file,
# this sub creates supplemental .rc and .RES files.
.sub 'generate_code_msvc'
    .param string pbc_path

    .local string rc_path
    .local string res_path
    rc_path  = 'replace_pbc_extension'(pbc_path, '.rc' )
    res_path = 'replace_pbc_extension'(pbc_path, '.res')

    # The exact numbers are not relevant;
    # they are used to identify the resource within the final executable.
    .local string rc_constant_defines
    rc_constant_defines = <<'END_OF_DEFINES'
#define RESOURCE_NAME_ID_WHOLE_PBC 333
#define RESOURCE_TYPE_ID_WHOLE_PBC 444
END_OF_DEFINES


    .local string rc_contents
    rc_contents  = ''
    rc_contents .= rc_constant_defines
    rc_contents .= 'RESOURCE_NAME_ID_WHOLE_PBC RESOURCE_TYPE_ID_WHOLE_PBC "'
    rc_contents .= pbc_path
    rc_contents .= "\"\n"

    .local pmc rc_fh
    rc_fh = new ['FileHandle']
    rc_fh.'open'(rc_path, 'w')
    unless rc_fh goto err_rc_open
    print rc_fh, rc_contents
    $I0 = rc_fh.'close'()
    unless $I0 == 0 goto err_rc_close


    .local int pbc_size
    $P0 = loadlib 'os'
    $P1 = new ['OS']
    $P2 = $P1.'stat'(pbc_path)
    pbc_size = $P2[7]


    .local pmc codestring
    codestring  = new [ 'StringBuilder' ]
    push codestring, "#include <windows.h>\n"
    push codestring, rc_constant_defines
    push codestring, "const unsigned int bytecode_size = "
    $S0 = pbc_size
    push codestring, $S0
    push codestring, ";\n"

    push codestring, <<'END_OF_FUNCTION'
        const void * get_program_code(void)
        {
            HRSRC   hResource;
            DWORD   size;
            HGLOBAL hPBC;
            LPVOID  actual_pointer_to_pbc_in_memory;

            hResource = FindResource(
                NULL,
                MAKEINTRESOURCE(RESOURCE_NAME_ID_WHOLE_PBC),
                MAKEINTRESOURCE(RESOURCE_TYPE_ID_WHOLE_PBC)
            );
            if (!hResource)
                return NULL;

            size = SizeofResource( NULL, hResource );
            if (size != bytecode_size)
                return NULL;

            hPBC = LoadResource( NULL, hResource );
            if (!hPBC)
                return NULL;

            actual_pointer_to_pbc_in_memory = LockResource( hPBC );
            if (!actual_pointer_to_pbc_in_memory)
                return NULL;

            return actual_pointer_to_pbc_in_memory;
        }
END_OF_FUNCTION

    .local string rc_cmd
    rc_cmd  = 'rc '
    rc_cmd .= rc_path

    say rc_cmd
    .local int status
    status = spawnw rc_cmd
    unless status goto rc_ok

    die "RC command failed"

  rc_ok:
    $S0 = codestring
    .return ($S0)

  err_h_open:
    die "cannot open .h file"
  err_rc_open:
    die "cannot open .rc file"
  err_h_close:
    die "cannot close .h file"
  err_rc_close:
    die "cannot close .rc file"
.end

# util functions
.sub 'compile_file'
    .param string cfile
    .param string objfile
    .param int    install

    $P0 = '_config'()
    .local string cc, ccflags, optimize, cc_o_out, osname, build_dir, slash
    .local string installed, includepath, versiondir
    cc        = $P0['cc']
    ccflags   = $P0['ccflags']
    optimize  = $P0['optimize']
    cc_o_out  = $P0['cc_o_out']
    osname    = $P0['osname']
    build_dir = $P0['build_dir']
    slash     = $P0['slash']
    installed = $P0['installed']
    includepath = $P0['includedir']
    versiondir = $P0['versiondir']

    .local string includedir, pathquote
    if installed == '1' goto installed_includedir
    includedir = concat build_dir, slash
    includedir = concat includedir, 'include'
    goto done_includedir
  installed_includedir:
    includedir = concat includepath, versiondir
  done_includedir:

    pathquote  = ''
    unless osname == 'MSWin32' goto not_windows
    pathquote  = '"'
  not_windows:

    .local string compile
    compile  = cc
    compile .= ' '
    compile .= cc_o_out
    compile .= objfile
    compile .= ' -I'
    compile .= pathquote
    compile .= includedir
    compile .= pathquote
    compile .= ' '
    compile .= ccflags
    compile .= ' '
    compile .= optimize
    compile .= ' -c '
    compile .= cfile

    say compile
    .local int status
    status = spawnw compile
    unless status goto compiled

    die "compilation failed"

  compiled:
    print "Compiled: "
    say objfile
    .return()
.end

.sub 'link_file'
    .param string objfile
    .param string exefile
    .param string extra_obj
    .param int    install

    $P0 = '_config'()
    .local string cc, link, link_dynamic, linkflags, ld_out, libparrot, libs, o
    .local string rpath, osname, build_dir, slash, icushared
    .local string installed, libdir, versiondir
    cc           = $P0['cc']
    link         = $P0['link']
    link_dynamic = $P0['link_dynamic']
    linkflags    = $P0['linkflags']
    ld_out       = $P0['ld_out']
    libparrot    = $P0['libparrot_linkflags']
    libs         = $P0['libs']
    o            = $P0['o']
    rpath        = $P0['rpath_blib']
    osname       = $P0['osname']
    build_dir    = $P0['build_dir']
    slash        = $P0['slash']
    icushared    = $P0['icu_shared']
    installed    = $P0['installed']
    libdir       = $P0['libdir']
    versiondir   = $P0['versiondir']

    .local string config, pathquote, exeprefix
    if installed == '1' goto config_installed
    exeprefix = substr exefile, 0, 12
    config     = concat build_dir, slash
    config    .= 'src'
    config    .= slash
    if install goto config_to_install
    config    .= 'parrot_config'
    goto config_cont
 config_to_install:
    config    .= 'install_config'
    rpath     = $P0['rpath_lib']
    goto config_cont
 config_installed:
    rpath      = $P0['rpath_lib']
    libparrot  = $P0['inst_libparrot_linkflags']
    config     = concat libdir, versiondir
    config    .= slash
    config    .= 'parrot_config'
 config_cont:
    config    .= o
    pathquote  = ''
    unless osname == 'MSWin32' goto not_windows
    pathquote  = '"'
  not_windows:

    link .= ' '
    link .= ld_out
    link .= exefile
    link .= ' '
    link .= pathquote
    link .= objfile
    link .= pathquote
    unless extra_obj > '' goto skip_extra_obj
    link .= ' '
    link .= pathquote
    link .= extra_obj
    link .= pathquote
  skip_extra_obj:
    link .= ' '
    link .= config
    link .= ' '
    link .= rpath
    link .= ' '
    link .= libparrot
    link .= ' '
    link .= link_dynamic
    link .= ' '
    link .= linkflags
    link .= ' '
    link .= libs
    link .= ' '
    link .= icushared

    say link
    .local int status
    status = spawnw link
    unless status goto check_manifest

    die "linking failed"

  check_manifest:
    # Check if there is a MSVC app manifest
    .local pmc file
    $P0 = loadlib 'file'
    file = new 'File'
    .local string manifest_file_name
    manifest_file_name  = clone exefile
    manifest_file_name .= '.manifest'
    .local pmc manifest_exists
    manifest_exists = file.'exists'( manifest_file_name )
    unless manifest_exists goto linked

  embed_manifest:
    # MSVC app manifest exists, embed it
    .local string embed_manifest_str
    embed_manifest_str  = 'mt.exe -nologo -manifest '
    embed_manifest_str .= manifest_file_name
    embed_manifest_str .= ' -outputresource:'
    embed_manifest_str .= exefile
    embed_manifest_str .= ';1'

    say embed_manifest_str
    .local int embed_manifest_status
    embed_manifest_status = spawnw embed_manifest_str
    unless embed_manifest_status goto linked
    die 'manifest embedding failed'

  linked:
    print "Linked: "
    say exefile
    .return()
.end

# handle any directory components
.sub 'prepend_installable'
    .param string file

    .local pmc path
    path     = split '/', file

    file     = path[-1]
    file     = concat 'installable_', file
    path[-1] = file

    file     = join '/', path

    .return( file )
.end

# Local Variables:
#   mode: pir
#   fill-column: 100
# End:
# vim: expandtab shiftwidth=4 ft=pir:
