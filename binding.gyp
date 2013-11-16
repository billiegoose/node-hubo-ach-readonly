{
  "targets": [
    {
      "target_name": "hubo_ach_readonly",
      "sources": [ "hubo_ach_readonly.cpp" ],
      'link_settings': {
        'libraries': ['-lach'],  # ACH
      }
    }
  ]
}
