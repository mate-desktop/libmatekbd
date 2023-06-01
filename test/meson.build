test_names = [
  'matekbd-indicator-test',
  'matekbd-keyboard-drawing-test',
  'matekbd-status-test',
]

foreach test_name : test_names
  test_exec = executable(
    test_name,
    test_name + '.c',
    dependencies: libmatekbdui_dep,
    c_args: [
      '-DGNOMELOCALEDIR="@0@"'.format (get_option('prefix') / get_option('datadir') / 'locale'),
    ],
    build_by_default: true,
  )
endforeach
