{
    'targets': [
        {
            'target_name': 'universe-native',
            'sources': [
                'src/convert.cc',
                'src/universe.cc',
                'src/is_alpha.cc'
            ],
            'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
            'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
            'cflags!': [ '-fno-exceptions' ],
            'cflags_cc!': [ '-fno-exceptions' ],
            "libraries": [ "../lib/libuvic.a", "-lffi" ],
            'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'CLANG_CXX_LIBRARY': 'libc++',
                'MACOSX_DEPLOYMENT_TARGET': '10.7'
            },
            'msvs_settings': {
                'VCCLCompilerTool': { 'ExceptionHandling': 1 },
            }
        }
    ]
}
