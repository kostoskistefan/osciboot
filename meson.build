project(
  'osciboot',
  'c',
  default_options : [
    'c_std=c99',
    'warning_level=2',
    'optimization=2'
  ],
)

executable('osciboot',
  sources : [
    'source/main.c',
    'source/menu.c',
    'source/font.c',
    'source/framebuffer.c',
    'source/configuration.c',
    'source/input_handler.c',
  ],
  include_directories : include_directories('include'),
  c_args : [
    '-DOSCIBOOT_DEBUG_TTY',
    '-D_POSIX_C_SOURCE=200809L',
	'-DVERSION_STRING=osciboot-v1.0.0 [x86_64]',
    '-DCONFIGURATION_VALIDATE_EXEC_FILE_EXISTS=0',
  ],
)
