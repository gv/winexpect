{
	'target_defaults': {
		'configurations': {
			'Debug': {
				'msvs_configuration_platform': 'x64',
			},
		},
		'msbuild_toolset': 'v141',
	},
	'targets':[{
		'target_name': 'winexpect',
		'type':'executable',
		'sources': ['expect.c', 'ui.c'],
        'libraries': ['-luser32', '-lgdi32', '-lshell32'],
		'msbuild_settings': {
			'ClCompile': {
				'RuntimeLibrary': 'MultiThreaded'
			},
			'Link': {	
				'SubSystem': 'windows'
			},
		}
	}]
}
