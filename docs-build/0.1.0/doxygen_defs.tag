<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.1" doxygen_gitid="ef9b20ac7f8a8621fcfc299f8bd0b80422390f4b">
  <compound kind="union">
    <name>LwpaIpAddr::AddrUnion</name>
    <filename>union_lwpa_ip_addr_1_1_addr_union.html</filename>
  </compound>
  <compound kind="struct">
    <name>lwpa_mutex_t</name>
    <filename>structlwpa__mutex__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>lwpa_rwlock_t</name>
    <filename>structlwpa__rwlock__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>lwpa_thread_t</name>
    <filename>structlwpa__thread__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>LwpaAddrinfo</name>
    <filename>struct_lwpa_addrinfo.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>ai_flags</name>
      <anchorfile>struct_lwpa_addrinfo.html</anchorfile>
      <anchor>a92528a848ebf34ab99687dd06a09cc93</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>ai_family</name>
      <anchorfile>struct_lwpa_addrinfo.html</anchorfile>
      <anchor>a4dc44d22f13bc5c59bff73e549e96a5c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>ai_socktype</name>
      <anchorfile>struct_lwpa_addrinfo.html</anchorfile>
      <anchor>a2109130e73586150c41fed16311e1af6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>ai_protocol</name>
      <anchorfile>struct_lwpa_addrinfo.html</anchorfile>
      <anchor>a6ade486587feaa03d492eb84cfc83451</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>ai_canonname</name>
      <anchorfile>struct_lwpa_addrinfo.html</anchorfile>
      <anchor>a9fcbdd64fcb9adcf195a78b10f1fe8f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaSockaddr</type>
      <name>ai_addr</name>
      <anchorfile>struct_lwpa_addrinfo.html</anchorfile>
      <anchor>a200d2e1203375035ef7d5c5c99e4a425</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pd</name>
      <anchorfile>struct_lwpa_addrinfo.html</anchorfile>
      <anchor>aecf048e565653706ca7120829d06e047</anchor>
      <arglist>[2]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaCid</name>
    <filename>struct_lwpa_cid.html</filename>
  </compound>
  <compound kind="struct">
    <name>LwpaIpAddr</name>
    <filename>struct_lwpa_ip_addr.html</filename>
    <class kind="union">LwpaIpAddr::AddrUnion</class>
    <member kind="variable">
      <type>lwpa_iptype_t</type>
      <name>type</name>
      <anchorfile>struct_lwpa_ip_addr.html</anchorfile>
      <anchor>a43f11ca9456afc383d0d550f16e3c420</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaLinger</name>
    <filename>struct_lwpa_linger.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>onoff</name>
      <anchorfile>struct_lwpa_linger.html</anchorfile>
      <anchor>a7730494d5c90b034d5937c758f354997</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>linger</name>
      <anchorfile>struct_lwpa_linger.html</anchorfile>
      <anchor>a21667e862207c7c116b20764fcb8f931</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaLogParams</name>
    <filename>struct_lwpa_log_params.html</filename>
    <member kind="variable">
      <type>lwpa_log_action_t</type>
      <name>action</name>
      <anchorfile>struct_lwpa_log_params.html</anchorfile>
      <anchor>a599599d80aee1a85e37ad61810456cc5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>lwpa_log_callback</type>
      <name>log_fn</name>
      <anchorfile>struct_lwpa_log_params.html</anchorfile>
      <anchor>a3e770b757979bd97f137d0801d3df5e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaSyslogParams</type>
      <name>syslog_params</name>
      <anchorfile>struct_lwpa_log_params.html</anchorfile>
      <anchor>ac3c58308b1be0a1a313137e208c15560</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>log_mask</name>
      <anchorfile>struct_lwpa_log_params.html</anchorfile>
      <anchor>ac695d621aaa836a9d13d65a912be9518</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>lwpa_log_time_fn</type>
      <name>time_fn</name>
      <anchorfile>struct_lwpa_log_params.html</anchorfile>
      <anchor>aeb908ea1d2aeaea8c56ef914948986cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>context</name>
      <anchorfile>struct_lwpa_log_params.html</anchorfile>
      <anchor>ae376f130b17d169ee51be68077a89ed0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaLogTimeParams</name>
    <filename>struct_lwpa_log_time_params.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>year</name>
      <anchorfile>struct_lwpa_log_time_params.html</anchorfile>
      <anchor>abeac221e38b7b9ce7df8722c842bf671</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>month</name>
      <anchorfile>struct_lwpa_log_time_params.html</anchorfile>
      <anchor>aedb06abe5aff12fa3e7e0e71a374edfb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>day</name>
      <anchorfile>struct_lwpa_log_time_params.html</anchorfile>
      <anchor>a4c11afc03fc3ee49bab660def6558f2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>hour</name>
      <anchorfile>struct_lwpa_log_time_params.html</anchorfile>
      <anchor>a15df9ba285cfd842f284025f904edc9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>minute</name>
      <anchorfile>struct_lwpa_log_time_params.html</anchorfile>
      <anchor>a5edffad982a0566ad01d95005474eae3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>second</name>
      <anchorfile>struct_lwpa_log_time_params.html</anchorfile>
      <anchor>a6cf35be1947a62f134392fcb1b3c54d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>msec</name>
      <anchorfile>struct_lwpa_log_time_params.html</anchorfile>
      <anchor>ad957f34cd6e8272740bc231865080b22</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>utc_offset</name>
      <anchorfile>struct_lwpa_log_time_params.html</anchorfile>
      <anchor>a0d5ceb6295477b5931bf54c002e36fe9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaMempool</name>
    <filename>struct_lwpa_mempool.html</filename>
  </compound>
  <compound kind="struct">
    <name>LwpaMempoolDesc</name>
    <filename>struct_lwpa_mempool_desc.html</filename>
    <member kind="variable">
      <type>const size_t</type>
      <name>elem_size</name>
      <anchorfile>struct_lwpa_mempool_desc.html</anchorfile>
      <anchor>a817b1b9f628c2698a9a3fb16ce738530</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const size_t</type>
      <name>pool_size</name>
      <anchorfile>struct_lwpa_mempool_desc.html</anchorfile>
      <anchor>aaeb9b2d37542fba1b318e4fcde49d9b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaMempool *</type>
      <name>freelist</name>
      <anchorfile>struct_lwpa_mempool_desc.html</anchorfile>
      <anchor>af38105b4ce33810fb92a498de0b73f63</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaMempool *const</type>
      <name>list</name>
      <anchorfile>struct_lwpa_mempool_desc.html</anchorfile>
      <anchor>abedf459e6fc547be7572a8fd4aecfbd5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>current_used</name>
      <anchorfile>struct_lwpa_mempool_desc.html</anchorfile>
      <anchor>aff331a9d3e0fdee5a3a504a3fe685bf0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *const</type>
      <name>pool</name>
      <anchorfile>struct_lwpa_mempool_desc.html</anchorfile>
      <anchor>aece14e36f8731b6f1c9602fee75c1544</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaMreq</name>
    <filename>struct_lwpa_mreq.html</filename>
    <member kind="variable">
      <type>LwpaIpAddr</type>
      <name>netint</name>
      <anchorfile>struct_lwpa_mreq.html</anchorfile>
      <anchor>aeb9189718092613b721ca97f43942753</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaIpAddr</type>
      <name>group</name>
      <anchorfile>struct_lwpa_mreq.html</anchorfile>
      <anchor>a9d92abf5e7c4488e5d9dfc5ab9376e89</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaNetintInfo</name>
    <filename>struct_lwpa_netint_info.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>ifindex</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>a01e636f8746c84f1cf3d45c4afbfde35</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaIpAddr</type>
      <name>addr</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>a070c3843626368d0129cf7295c10296e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaIpAddr</type>
      <name>mask</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>abff5b2b0f50f02db2d130bb3c59cd5b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaIpAddr</type>
      <name>gate</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>aab794a1d7268e4cf399af0600c73e0d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>mac</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>acfb2730f6bc990ea60b006dd2bf6e2b2</anchor>
      <arglist>[NETINTINFO_MAC_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>name</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>aa1880b44aa15396b0f044c0a84d2b0f3</anchor>
      <arglist>[NETINTINFO_NAME_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>is_default</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>a0c9f19a7b68ad0dad836ee153d3a230a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaPdu</name>
    <filename>struct_lwpa_pdu.html</filename>
  </compound>
  <compound kind="struct">
    <name>LwpaPduConstraints</name>
    <filename>struct_lwpa_pdu_constraints.html</filename>
    <member kind="variable">
      <type>size_t</type>
      <name>vector_size</name>
      <anchorfile>struct_lwpa_pdu_constraints.html</anchorfile>
      <anchor>a28e18243c88fd99888246e855cc116fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>header_size</name>
      <anchorfile>struct_lwpa_pdu_constraints.html</anchorfile>
      <anchor>a64f1d0c53909f30b0aac882348cd32f2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaPollfd</name>
    <filename>struct_lwpa_pollfd.html</filename>
    <member kind="variable">
      <type>lwpa_socket_t</type>
      <name>fd</name>
      <anchorfile>struct_lwpa_pollfd.html</anchorfile>
      <anchor>a9a0209807bcf09911892e425cf8bc003</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>short</type>
      <name>events</name>
      <anchorfile>struct_lwpa_pollfd.html</anchorfile>
      <anchor>a82463718695c17094aa41c974eb35d61</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>short</type>
      <name>revents</name>
      <anchorfile>struct_lwpa_pollfd.html</anchorfile>
      <anchor>ae9f68e4cc4edb4a9f8413e62e6167fde</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>lwpa_error_t</type>
      <name>err</name>
      <anchorfile>struct_lwpa_pollfd.html</anchorfile>
      <anchor>a2d84f39897e3cbaf81be57d00dce398b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaRbIter</name>
    <filename>struct_lwpa_rb_iter.html</filename>
    <member kind="variable">
      <type>LwpaRbTree *</type>
      <name>tree</name>
      <anchorfile>struct_lwpa_rb_iter.html</anchorfile>
      <anchor>ac7b99c0a7b935ddc4664f3301c3642d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaRbNode *</type>
      <name>node</name>
      <anchorfile>struct_lwpa_rb_iter.html</anchorfile>
      <anchor>aa53715d827987acdffce1862294c8349</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaRbNode *</type>
      <name>path</name>
      <anchorfile>struct_lwpa_rb_iter.html</anchorfile>
      <anchor>a431a8b3f6f36f5c3e9f381a42bfabc5a</anchor>
      <arglist>[RB_ITER_MAX_HEIGHT]</arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>top</name>
      <anchorfile>struct_lwpa_rb_iter.html</anchorfile>
      <anchor>ab9332a87d0af5aa12cac847eff68b7af</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>info</name>
      <anchorfile>struct_lwpa_rb_iter.html</anchorfile>
      <anchor>acb1df3a0f703b05bc4971f79cabe2597</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaRbNode</name>
    <filename>struct_lwpa_rb_node.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>red</name>
      <anchorfile>struct_lwpa_rb_node.html</anchorfile>
      <anchor>a6761340706096510fd89edca40a63b9b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaRbNode *</type>
      <name>link</name>
      <anchorfile>struct_lwpa_rb_node.html</anchorfile>
      <anchor>a791da83334fff8f52b7f4f4fbe0f4020</anchor>
      <arglist>[2]</arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>value</name>
      <anchorfile>struct_lwpa_rb_node.html</anchorfile>
      <anchor>a0f61d63b009d0880a89c843bd50d8d76</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaRbTree</name>
    <filename>struct_lwpa_rb_tree.html</filename>
    <member kind="variable">
      <type>LwpaRbNode *</type>
      <name>root</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>a3c22ec40f8e99862b2a75a030e279648</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>rb_tree_node_cmp_f</type>
      <name>cmp</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>a3b4d0c1a0e7f6eac13267a7ae55d4179</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>size</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>a854352f53b148adc24983a58a1866d66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>rb_node_alloc_f</type>
      <name>alloc_f</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>a93e173200002f4685f94f14d4eb674e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>rb_node_dealloc_f</type>
      <name>dealloc_f</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>ab3e9daf21d6483195063b31b3507f82e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>info</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>acb1df3a0f703b05bc4971f79cabe2597</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaSockaddr</name>
    <filename>struct_lwpa_sockaddr.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>port</name>
      <anchorfile>struct_lwpa_sockaddr.html</anchorfile>
      <anchor>a8e0798404bf2cf5dabb84c5ba9a4f236</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaIpAddr</type>
      <name>ip</name>
      <anchorfile>struct_lwpa_sockaddr.html</anchorfile>
      <anchor>aaafd70d57737ab374198d3d754411129</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>scope_id</name>
      <anchorfile>struct_lwpa_sockaddr.html</anchorfile>
      <anchor>a1d93077fdccdb6e377b421b79e1ba15c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaSocketPlatformData</name>
    <filename>struct_lwpa_socket_platform_data.html</filename>
  </compound>
  <compound kind="struct">
    <name>LwpaSyslogParams</name>
    <filename>struct_lwpa_syslog_params.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>facility</name>
      <anchorfile>struct_lwpa_syslog_params.html</anchorfile>
      <anchor>a213afa2e73e0c2102f43d4067e41b90f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>hostname</name>
      <anchorfile>struct_lwpa_syslog_params.html</anchorfile>
      <anchor>a101da4fbfd94ee9467e2931768d64488</anchor>
      <arglist>[LWPA_LOG_HOSTNAME_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>app_name</name>
      <anchorfile>struct_lwpa_syslog_params.html</anchorfile>
      <anchor>ac56492caa804aa3574dec4e755384200</anchor>
      <arglist>[LWPA_LOG_APP_NAME_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>procid</name>
      <anchorfile>struct_lwpa_syslog_params.html</anchorfile>
      <anchor>a08aa82e27aff5eddbfc578f623f54f4a</anchor>
      <arglist>[LWPA_LOG_PROCID_MAX_LEN]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaThreadParams</name>
    <filename>struct_lwpa_thread_params.html</filename>
    <member kind="variable">
      <type>unsigned int</type>
      <name>thread_priority</name>
      <anchorfile>struct_lwpa_thread_params.html</anchorfile>
      <anchor>a9cc3e9ae3970c47e150dc77065abd75f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned int</type>
      <name>stack_size</name>
      <anchorfile>struct_lwpa_thread_params.html</anchorfile>
      <anchor>adde5266300e9cdd7ca1134daba9adf24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>thread_name</name>
      <anchorfile>struct_lwpa_thread_params.html</anchorfile>
      <anchor>aff53d286a69bd6b453eeb29ab3a75326</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>platform_data</name>
      <anchorfile>struct_lwpa_thread_params.html</anchorfile>
      <anchor>abea5d4a9dc1ef714b423a1ae6afaf4ae</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaThreadParamsMqx</name>
    <filename>struct_lwpa_thread_params_mqx.html</filename>
  </compound>
  <compound kind="struct">
    <name>LwpaTimer</name>
    <filename>struct_lwpa_timer.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>reset_time</name>
      <anchorfile>struct_lwpa_timer.html</anchorfile>
      <anchor>a8fb66f63094794fbbdacc69af5cc55fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>interval</name>
      <anchorfile>struct_lwpa_timer.html</anchorfile>
      <anchor>acd9ed1ccc44de55e1d3eff5569663695</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaUid</name>
    <filename>struct_lwpa_uid.html</filename>
  </compound>
  <compound kind="struct">
    <name>MD5_CTX</name>
    <filename>struct_m_d5___c_t_x.html</filename>
  </compound>
  <compound kind="struct">
    <name>RootLayerPdu</name>
    <filename>struct_root_layer_pdu.html</filename>
    <member kind="variable">
      <type>LwpaCid</type>
      <name>sender_cid</name>
      <anchorfile>struct_root_layer_pdu.html</anchorfile>
      <anchor>aa637edf35bbbdf0fceb946c870aca115</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>vector</name>
      <anchorfile>struct_root_layer_pdu.html</anchorfile>
      <anchor>a513d751c7097b43b3968ac81f3a5715b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pdata</name>
      <anchorfile>struct_root_layer_pdu.html</anchorfile>
      <anchor>a2f9eb29aa10e0d64f27f69db1e90bd07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>datalen</name>
      <anchorfile>struct_root_layer_pdu.html</anchorfile>
      <anchor>a3d12e161726566d24a1d0cf6c6b416bb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tagTHREADNAME_INFO</name>
    <filename>structtag_t_h_r_e_a_d_n_a_m_e___i_n_f_o.html</filename>
  </compound>
  <compound kind="struct">
    <name>TcpPreamble</name>
    <filename>struct_tcp_preamble.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>rlp_block</name>
      <anchorfile>struct_tcp_preamble.html</anchorfile>
      <anchor>a6df9d0f27538da177c52996a97e0a1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rlp_block_len</name>
      <anchorfile>struct_tcp_preamble.html</anchorfile>
      <anchor>ab27f79ca664937f91e64f868ad233935</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>UdpPreamble</name>
    <filename>struct_udp_preamble.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>rlp_block</name>
      <anchorfile>struct_udp_preamble.html</anchorfile>
      <anchor>a6df9d0f27538da177c52996a97e0a1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rlp_block_len</name>
      <anchorfile>struct_udp_preamble.html</anchorfile>
      <anchor>ab27f79ca664937f91e64f868ad233935</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_lock</name>
    <title>lwpa_lock</title>
    <filename>group__lwpa__lock.html</filename>
    <subgroup>lwpa_mutex</subgroup>
    <subgroup>lwpa_signal</subgroup>
    <subgroup>lwpa_rwlock</subgroup>
  </compound>
  <compound kind="group">
    <name>lwpa_mutex</name>
    <title>lwpa_mutex</title>
    <filename>group__lwpa__mutex.html</filename>
    <member kind="typedef">
      <type>UNDEFINED</type>
      <name>lwpa_mutex_t</name>
      <anchorfile>group__lwpa__mutex.html</anchorfile>
      <anchor>gadd901205c027cd6c51e26270d34b9a29</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_mutex_create</name>
      <anchorfile>group__lwpa__mutex.html</anchorfile>
      <anchor>ga33eb7f5c98da5276a454e906cafeea87</anchor>
      <arglist>(lwpa_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_mutex_take</name>
      <anchorfile>group__lwpa__mutex.html</anchorfile>
      <anchor>ga7726931cda1cdbf1307283b3894c4af2</anchor>
      <arglist>(lwpa_mutex_t *id, int wait_ms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_mutex_give</name>
      <anchorfile>group__lwpa__mutex.html</anchorfile>
      <anchor>ga2d9b3e1d0507bc519c6da8cf1c1981f4</anchor>
      <arglist>(lwpa_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_mutex_destroy</name>
      <anchorfile>group__lwpa__mutex.html</anchorfile>
      <anchor>gac4684dd66997cab2515ae883b5ac51d8</anchor>
      <arglist>(lwpa_mutex_t *id)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_signal</name>
    <title>lwpa_signal</title>
    <filename>group__lwpa__signal.html</filename>
    <member kind="typedef">
      <type>UNDEFINED</type>
      <name>lwpa_signal_t</name>
      <anchorfile>group__lwpa__signal.html</anchorfile>
      <anchor>gaad04ad0c8d71e1b5b6a4e363965e43ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_signal_create</name>
      <anchorfile>group__lwpa__signal.html</anchorfile>
      <anchor>ga7e2c2e183c84b10d80c57fccdd49c9f7</anchor>
      <arglist>(lwpa_signal_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_signal_wait</name>
      <anchorfile>group__lwpa__signal.html</anchorfile>
      <anchor>ga64d1e2d7b517cdb5f2c3d33081ff6d7c</anchor>
      <arglist>(lwpa_signal_t *id, int wait_ms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_signal_post</name>
      <anchorfile>group__lwpa__signal.html</anchorfile>
      <anchor>gab967464bd5c6bd05d8d110155777159c</anchor>
      <arglist>(lwpa_signal_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_signal_destroy</name>
      <anchorfile>group__lwpa__signal.html</anchorfile>
      <anchor>ga1e76d7a093cd92f01b15f8a185726399</anchor>
      <arglist>(lwpa_signal_t *id)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_rwlock</name>
    <title>lwpa_rwlock</title>
    <filename>group__lwpa__rwlock.html</filename>
    <member kind="typedef">
      <type>UNDEFINED</type>
      <name>lwpa_rwlock_t</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>ga74e912ed8c5b1c545139f8cd6568ae45</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_rwlock_create</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>ga84a16e1e575e5bef32c0f5d50725b7a1</anchor>
      <arglist>(lwpa_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_rwlock_readlock</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>gab5db36f592b621b305c7808c1496a3ae</anchor>
      <arglist>(lwpa_rwlock_t *id, int wait_ms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_rwlock_readunlock</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>gaa2311356e0d51571c0560262468b0089</anchor>
      <arglist>(lwpa_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_rwlock_writelock</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>ga33b3938c8392576c3d780f759c975ec5</anchor>
      <arglist>(lwpa_rwlock_t *id, int wait_ms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_rwlock_writeunlock</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>ga33dd48c0866e13f3bccbaffc8d227b9c</anchor>
      <arglist>(lwpa_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_rwlock_destroy</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>ga5422591a8e0f2f3ca52816e531fc9815</anchor>
      <arglist>(lwpa_rwlock_t *id)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_thread</name>
    <title>lwpa_thread</title>
    <filename>group__lwpa__thread.html</filename>
    <class kind="struct">LwpaThreadParams</class>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_THREAD_DEFAULT_PRIORITY</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>gabe69c3e7a2ee99bec3517c2afb0c6b76</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_THREAD_DEFAULT_STACK</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>gade705a86763d0db86eb78ff6c58aed4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_THREAD_DEFAULT_NAME</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>gab11c37b97da30254b66ecaf215062632</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_THREAD_NAME_MAX_LENGTH</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>gad7eb7c6d53f153f8e9fed7a35bfd9d95</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaThreadParams</type>
      <name>LwpaThreadParams</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>gaa3ccf9778a39583135ac168668a9ab1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>UNDEFINED</type>
      <name>lwpa_thread_t</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>ga772303b983e1df853a66868f1419257c</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_thread_create</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>gac3533f2da6b083858517fcb2ba660649</anchor>
      <arglist>(lwpa_thread_t *id, const LwpaThreadParams *params, void(*thread_fn)(void *), void *thread_arg)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_thread_stop</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>gac98ab5b271d1fdf59b2df4d04f38f7f3</anchor>
      <arglist>(lwpa_thread_t *id, int wait_ms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_thread_sleep</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>ga1b3993fb872deb53b0129d43429b5cb1</anchor>
      <arglist>(int sleep_ms)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_bool</name>
    <title>lwpa_bool</title>
    <filename>group__lwpa__bool.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>bool</name>
      <anchorfile>group__lwpa__bool.html</anchorfile>
      <anchor>gabb452686968e48b67397da5f97445f5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>false</name>
      <anchorfile>group__lwpa__bool.html</anchorfile>
      <anchor>ga65e9886d74aaee76545e83dd09011727</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>true</name>
      <anchorfile>group__lwpa__bool.html</anchorfile>
      <anchor>ga41f9c5fb8b08eb5dc3edce4dcb37fee7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_cid</name>
    <title>lwpa_cid</title>
    <filename>group__lwpa__cid.html</filename>
    <class kind="struct">LwpaCid</class>
    <member kind="define">
      <type>#define</type>
      <name>CID_BYTES</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>gaee95c27b36279244f3037cee3de2f16f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>cidcmp</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>ga9a3d36611766c680c40e0742551af114</anchor>
      <arglist>(cid1ptr, cid2ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>cid_isnull</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>ga282b353a04c10c13968db9b32e1a71fc</anchor>
      <arglist>(cidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CID_STRING_BYTES</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>ga1f424d01d1f4390897bf75c224c87979</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaCid</type>
      <name>LwpaCid</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>gaf72b37cf63fc8bee55393c5f46c9d374</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cid_to_string</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>ga922ba9e1a9da548623280193706e8800</anchor>
      <arglist>(char *buf, const LwpaCid *cid)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>string_to_cid</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>ga18bf762e02022b61d1b63de8e66728bc</anchor>
      <arglist>(LwpaCid *cid, const char *buf, size_t buflen)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>generate_cid</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>ga2b90d5c4c01894a8f0892e715f880974</anchor>
      <arglist>(LwpaCid *cid, const char *devstr, const uint8_t *macaddr, uint32_t cidnum)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>generate_rdm_cid</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>ga44ac6bfbb401e50d62a33a6b5d83737f</anchor>
      <arglist>(LwpaCid *cid, const uint8_t *rdmuid)</arglist>
    </member>
    <member kind="variable">
      <type>const LwpaCid</type>
      <name>NULL_CID</name>
      <anchorfile>group__lwpa__cid.html</anchorfile>
      <anchor>ga1fab4750423a1e3e6efa2c58ef19c5eb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa</name>
    <title>lwpa</title>
    <filename>group__lwpa.html</filename>
    <subgroup>lwpa_lock</subgroup>
    <subgroup>lwpa_thread</subgroup>
    <subgroup>lwpa_bool</subgroup>
    <subgroup>lwpa_cid</subgroup>
    <subgroup>lwpa_error</subgroup>
    <subgroup>lwpa_inet</subgroup>
    <subgroup>lwpa_int</subgroup>
    <subgroup>lwpa_log</subgroup>
    <subgroup>lwpa_mempool</subgroup>
    <subgroup>lwpa_netint</subgroup>
    <subgroup>lwpa_pack</subgroup>
    <subgroup>lwpa_pdu</subgroup>
    <subgroup>lwpa_rbtree</subgroup>
    <subgroup>lwpa_socket</subgroup>
    <subgroup>lwpa_timer</subgroup>
    <subgroup>lwpa_uid</subgroup>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_WAIT_FOREVER</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>ga1c2b0e972201228ff6c55879bab82861</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_error</name>
    <title>lwpa_error</title>
    <filename>group__lwpa__error.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_strerror</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gadd5acb6baf8584951dd378c7e92ddf54</anchor>
      <arglist>(errcode)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>lwpa_error_t</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>ga4de447127cf0baaa9afb0188e1e6afbf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_OK</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa857ed78ba3e816c71b69c16c85c21647</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_NOTFOUND</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa2acbe19477fb07c07db1f9b3d9bb0637</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_NOMEM</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa1c83c6b26dacaebf3ff5ee52fb85e4c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_BUSY</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfab105c4c9af4d26afbd6dcb664279770a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_EXIST</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa219cade9f19d3b17ff65b56d98f41eee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_INVALID</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa9cf2f92a879c1eacc80a024bae41cd96</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_WOULDBLOCK</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa75f1c87405d6317c354d5e8e87bba817</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_NODATA</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa06ed657852ed6acda52f334c2bc87ad0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_PROTERR</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa60397db549f16a805fb4553d0cc2d5d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_MSGSIZE</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa854b24c13e687336e7cb4ae0110866f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_ADDRINUSE</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa254f03432a42298cb3caded4644763d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_ADDRNOTAVAIL</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfacca2125a6d3fe7e2defd93e0364326a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_NETERR</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfaf65425d05b3d91f90adf5f0ee7dc8092</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_CONNRESET</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfaad3a7c2661b83f69312d25767d7455d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_CONNCLOSED</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfaaa097ac19c3cf75639f475aca2c59971</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_ISCONN</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfad4194b82d4385ca8b95136fcf542e76c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_NOTCONN</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa92048423fc209e99bf80a9685e0824a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_SHUTDOWN</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa7ccf319f032b55fd13ddfb4c319d5028</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_TIMEDOUT</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa227b5a6ccb818da7bcc74cecbf193f7f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_CONNREFUSED</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa5d5cc5181b64b6aed02cd510cf084d5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_ALREADY</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa543a4059f06676e225762b0f21f945e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_INPROGRESS</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa11a9a2879288ced39e9091d61c5eb723</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_BUFSIZE</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa1600093d7a7190c396cae1b19a34ed74</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_NOTINIT</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa093ea9e89008867f7049d4203ffd31cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_NOIFACES</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa5183c038cc84e7cbaab3399f7f76a1bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_NOTIMPL</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa051f09b950e5709f3f2a1cc4e32a08a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_SYSERR</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfabc09b1c02b9178906e1b6984f9021ad4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_inet</name>
    <title>lwpa_inet</title>
    <filename>group__lwpa__inet.html</filename>
    <class kind="struct">LwpaIpAddr</class>
    <class kind="struct">LwpaSockaddr</class>
    <class kind="struct">LwpaNetintInfo</class>
    <member kind="define">
      <type>#define</type>
      <name>IPV6_BYTES</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gad13014eeb2e4a08990536a16089be0f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpasock_ip_port_equal</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gacdc07c9ddec218067a54d87f017b256c</anchor>
      <arglist>(sockptr1, sockptr2)</arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaIpAddr</type>
      <name>LwpaIpAddr</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gaab91649a1b40a20f67fe86472e6f42f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaSockaddr</type>
      <name>LwpaSockaddr</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gac30c14a5e951cfcb8e66086a7779e7d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaNetintInfo</type>
      <name>LwpaNetintInfo</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga24575fba7debad61e88da1061ae77837</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>lwpa_iptype_t</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga75918e41a21a5b4511b2b3c797e6f048</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_IP_INVALID</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gga75918e41a21a5b4511b2b3c797e6f048a827878502eeeef634e5d0d4dd3c77e88</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_IPV4</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gga75918e41a21a5b4511b2b3c797e6f048af03dcc1f24060aa236145f272fbf88cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LWPA_IPV6</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gga75918e41a21a5b4511b2b3c797e6f048a474b73f5d6671c188f851d9c1d3c6a0d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_is_v4</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga99ca443ff7adfc20ef3dfff1f668a563</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_is_v6</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga60fa1dee3547985e5419d7f3d4f8b233</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_is_invalid</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga512afd056b57c62b511e44a93972f4d5</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_v4_address</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga731ba4e4b7760993fe3f4658066b9fcd</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_v6_address</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga6f3e7c1f5539712019154ffeed900d07</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_set_v4_address</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga8cefecedef40877115c050848fd9a1d5</anchor>
      <arglist>(lwpa_ip_ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_set_v6_address</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga82919752f2428a78edfb26f745481e5d</anchor>
      <arglist>(lwpa_ip_ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_set_invalid</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga85259d4019be76ee5067cb744cb4cb76</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_is_multicast</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga2a3be69f1ef99486ad72ab4b8eae966d</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_equal</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga1961dd86bd985153be3afa3bf3bebefa</anchor>
      <arglist>(ipptr1, ipptr2)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_cmp</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga7bcd89c614dbfb588d5c6bb54ce67872</anchor>
      <arglist>(ipptr1, ipptr2)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_make_any_v4</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gafff526c280a904c4f27372a000e37859</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpaip_make_any_v6</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gaf0433f37c8aedbd049388bb7c08f5172</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_int</name>
    <title>lwpa_int</title>
    <filename>group__lwpa__int.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>HAVE_STDINT_H</name>
      <anchorfile>group__lwpa__int.html</anchorfile>
      <anchor>gab6cd6d1c63c1e26ea2d4537b77148354</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_log</name>
    <title>lwpa_log</title>
    <filename>group__lwpa__log.html</filename>
    <class kind="struct">LwpaLogTimeParams</class>
    <class kind="struct">LwpaSyslogParams</class>
    <class kind="struct">LwpaLogParams</class>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_NFACILITIES</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga9ddc5e6a9583f9d8d12e9fddd5973461</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_HOSTNAME_MAX_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gac283f15a55e1f885b4a284cdf749a318</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_APP_NAME_MAX_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaea6845af17f90339030b7f1aed4248b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_PROCID_MAX_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaf444c37d4b37d0d12a01cdd900c54343</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_MSG_MAX_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga3c6145528eb237caccd1e364ed0574c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_TIMESTAMP_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gae9a41c527892d020281abfb31f4f01e0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SYSLOG_HEADER_MAX_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaef6b15895880ea225057a79bbb120945</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SYSLOG_STR_MIN_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga98a9352b7969f965cf4c77af3c462f21</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_HUMAN_LOG_STR_MIN_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga794bc9c3568fda36ad6e26a790ae6195</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SYSLOG_STR_MAX_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga8cd1e1b2619e012f10e62b91f3574e27</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_HUMAN_LOG_STR_MAX_LEN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga0495001df9d738759753ecb7c9f4772a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaLogTimeParams</type>
      <name>LwpaLogTimeParams</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga606f589b43fe2179920d524dd7fae794</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>lwpa_log_callback</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gacd26df727c6a5f6d0d6fcfa4c2ac3755</anchor>
      <arglist>)(void *context, const char *syslog_str, const char *human_str, const char *raw_str)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>lwpa_log_time_fn</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga663d6e629850d5957be6634468f04755</anchor>
      <arglist>)(void *context, LwpaLogTimeParams *time_params)</arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaSyslogParams</type>
      <name>LwpaSyslogParams</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gabdd66ba1cf4cf44332a3e6bf42605a00</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaLogParams</type>
      <name>LwpaLogParams</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga39f06f5905acfcf5c957112603eeb96c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>lwpa_log_action_t</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga87102d7ecdab1a3ba919bd750fae0507</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaLogCreateSyslog</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gga87102d7ecdab1a3ba919bd750fae0507a408d0b541c1860eb8264be42f2d0d774</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaLogCreateHumanReadableLog</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gga87102d7ecdab1a3ba919bd750fae0507a21fba732cbc622fce83415718e1fb8f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaLogCreateBoth</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gga87102d7ecdab1a3ba919bd750fae0507a1c00ba8139d620d17be3b61a498232c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_create_syslog_str</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga2efb597d28fdda5796e410b43f5a631d</anchor>
      <arglist>(char *buf, size_t buflen, const LwpaLogTimeParams *time, const LwpaSyslogParams *syslog_params, int pri, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_create_human_log_str</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga92091520538853c272084261bbb8bf21</anchor>
      <arglist>(char *buf, size_t buflen, const LwpaLogTimeParams *time, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_sanitize_syslog_params</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga2e1cd052c474bfbb33614a7d10b33bbb</anchor>
      <arglist>(LwpaSyslogParams *params)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_validate_log_params</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga745327935a6efcc73f2f5048757c6de1</anchor>
      <arglist>(LwpaLogParams *params)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_log</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga6584e9d020e7cf99caaca6bf9acf81ef</anchor>
      <arglist>(const LwpaLogParams *params, int pri, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_vlog</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga4d3b4176e7d17c30aaa9dd82b6c5c3da</anchor>
      <arglist>(const LwpaLogParams *params, int pri, const char *format, va_list args)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_KERN</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaf25b2f84f136f35d7daf3bca75c09b39</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_USER</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga08e3706b92e3aeec1c44769c42fc18b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_MAIL</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga04d4892d83f2084956ebe6ca8c479500</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_DAEMON</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaca7e4a2edd294cea9c23ebf18c881f41</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_AUTH</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga0e5d159ba215deea5f706fa33d71738e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_SYSLOG</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga2441590d39aaca7c88136145b0eb880b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LPR</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gacbf6ea806a67f53e17d265d9f7d8c0e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_NEWS</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga20f59aff1e3c478dc610c6bb3f1cf30c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_UUCP</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga3374f025fafa38717f707a047359fd30</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_CRON</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga1cce65659ecc6a47c1d5aa7071ae69f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_AUTHPRIV</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaa50dfd3d27e5f3774232b0271bb58e90</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_FTP</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gadcfd4e54bed9d3ee05b5376822f409a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LOCAL0</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gae3d1d7327370707be0202b28c26ce4a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LOCAL1</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaa99273c70b0d7f39eeb53b1bc6d4280b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LOCAL2</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gac67f31952e97f767d74ec9d2f644759a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LOCAL3</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gadea7fe8c18631a76bc0badd13c9b8d80</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LOCAL4</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga8062bd367e9dcfc7e0479bbf07a6f759</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LOCAL5</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga68947f6ab3d2c55c0ecb8bf37d88d9d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LOCAL6</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gacf65fe6eff3a5b59a345066345572fb2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_LOCAL7</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga0c3309dc6bfbe192e61b55ee23e903bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_EMERG</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaa211f156cab5f49438cdfa9e44b63e31</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_ALERT</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga4e4bdf39335b35ef2f878abaea51dfcc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_CRIT</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga8ff207c1ddab5e2df910cc4fe464c662</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_ERR</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga3d042c56486e03eb5b187a335ff5101a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_WARNING</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga68217ab863c4cd43c7ff03ca2219c47b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_NOTICE</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaf6777f9e9a8f38724b151304d1404319</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_INFO</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gaddc8c1082db67e72912a74b669d29c23</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_LOG_DEBUG</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga4a8948b531b0cf1c68a1bdc5c84d4ceb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_mempool</name>
    <title>lwpa_mempool</title>
    <filename>group__lwpa__mempool.html</filename>
    <class kind="struct">LwpaMempool</class>
    <class kind="struct">LwpaMempoolDesc</class>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_MEMPOOL_DECLARE</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>ga94986ea8060c07cba61c85b2a99ed458</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_MEMPOOL_DEFINE</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>gaa0d4858b58e87161808339beba9f4c87</anchor>
      <arglist>(name, type, size)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_mempool_init</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>gaef04cee1ba289720a3a5701248ac6141</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_mempool_alloc</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>ga974021021787bdc6dd16e5017869a556</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_mempool_free</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>ga1962ba16f32cecfafc8ba4b60765c0de</anchor>
      <arglist>(name, mem)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_mempool_size</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>ga329d6610b1bb2e7ac37fc8f7ce77d7a2</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_mempool_used</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>gae4d0c61601e666706baab21642f11742</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaMempool</type>
      <name>LwpaMempool</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>ga61998e19568afbba25a80d83ef22e380</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaMempoolDesc</type>
      <name>LwpaMempoolDesc</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>gaa56c6c42a0d774c5bfa25f2bacbcfc94</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_netint</name>
    <title>lwpa_netint</title>
    <filename>group__lwpa__netint.html</filename>
    <member kind="function">
      <type>size_t</type>
      <name>netint_get_num_interfaces</name>
      <anchorfile>group__lwpa__netint.html</anchorfile>
      <anchor>ga6b5fd1732b51d2090225617c87a902b8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>netint_get_interfaces</name>
      <anchorfile>group__lwpa__netint.html</anchorfile>
      <anchor>ga5a175a38550ad3b30bb0930a279311ee</anchor>
      <arglist>(LwpaNetintInfo *netint_arr, size_t netint_arr_size)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>netint_get_default_interface</name>
      <anchorfile>group__lwpa__netint.html</anchorfile>
      <anchor>gaae95e6b2b89d004506ac868d094d0ac0</anchor>
      <arglist>(LwpaNetintInfo *netint)</arglist>
    </member>
    <member kind="function">
      <type>const LwpaNetintInfo *</type>
      <name>netint_get_iface_for_dest</name>
      <anchorfile>group__lwpa__netint.html</anchorfile>
      <anchor>ga21a8bd8c4fd3fd9be340a6f9051eacc7</anchor>
      <arglist>(const LwpaIpAddr *dest, const LwpaNetintInfo *netint_arr, size_t netint_arr_size)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_pack</name>
    <title>lwpa_pack</title>
    <filename>group__lwpa__pack.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>upack_16b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga68d1fab4542e3bfde42b2e071078024d</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pack_16b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga32f0584d0100019465abdae96fced391</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>upack_16l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga38fbf13e5a30a30bfc16f8849b9c0e13</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pack_16l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gaeff170e309ac2e9eed5bee5b97258728</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>upack_32b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga442155eb9bd7cb4312b004b50aa39608</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pack_32b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gabc8cfafb05fb11c81e8f5dbc89472de7</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>upack_32l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga3b8f297f4976651c00e2dd15fae2c2b4</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pack_32l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga093d966b9ca2bd2669c04087e27ca1ee</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>upack_64b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gaeff4455ed41104dd89cf0330bf05f487</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pack_64b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gaafd5b33653239f0f842b05b3d1f51ff4</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>upack_64l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gaa321477a84b0d802fae691ab8a0bba5a</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pack_64l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gab0c737df215c1bafd0012f5f2b6e75fe</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_pdu</name>
    <title>lwpa_pdu</title>
    <filename>group__lwpa__pdu.html</filename>
    <subgroup>lwpa_rootlayerpdu</subgroup>
    <class kind="struct">LwpaPdu</class>
    <class kind="struct">LwpaPduConstraints</class>
    <member kind="define">
      <type>#define</type>
      <name>pdu_length</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga26b92b4dfc472ca15e229260aae48592</anchor>
      <arglist>(pdu_buf)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pdu_pack_normal_len</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gabef4551d2056e46f2e9288480cdf20c4</anchor>
      <arglist>(pdu_buf, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pdu_pack_ext_len</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gad9b9ee28c2c3923ca1ece7239eff6b7d</anchor>
      <arglist>(pdu_buf, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PDU_INIT</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga5263d5874200e90de739e1cf453078ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>init_pdu</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gaa750a6d6dbdffd9d35f20c608aa71529</anchor>
      <arglist>(pduptr)</arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaPdu</type>
      <name>LwpaPdu</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga5263af6d575fa157acea872c13390372</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaPduConstraints</type>
      <name>LwpaPduConstraints</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga16efbf331bb58000884ba675a45bc715</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>parse_pdu</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga099a70bb62e8eeb7bcd338c6c2565b9f</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, const LwpaPduConstraints *constraints, LwpaPdu *pdu)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>l_flag_set</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga7b46c39b50be48a8253dd21ab6a58b91</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>v_flag_set</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga4173e8e7c7e466c0838d444ef380fb88</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>h_flag_set</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gaded5a889b6168f4dba687b319bb3ce3a</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>d_flag_set</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga21c0bdfc1d6580d6a7f4067c5f0e2a22</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>set_l_flag</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gaa4c290b814c7db454dd912abc2712612</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>set_v_flag</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga06ef097d7a1f8efa9033660edb3b2038</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>set_h_flag</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gaa51d5354237eac56446b124e7c77b605</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>set_d_flag</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gaea2405bfe6dd376950dbb72fe36bb56e</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_rbtree</name>
    <title>lwpa_rbtree</title>
    <filename>group__lwpa__rbtree.html</filename>
    <class kind="struct">LwpaRbNode</class>
    <class kind="struct">LwpaRbTree</class>
    <class kind="struct">LwpaRbIter</class>
    <member kind="define">
      <type>#define</type>
      <name>RB_ITER_MAX_HEIGHT</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga0d5d812c77767d9b18de1620e34167ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaRbIter</type>
      <name>LwpaRbIter</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gaadc1dd0e9c4ca0183ec47958b977e376</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rb_tree_node_cmp_ptr_cb</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga81b54f02c74208cebaf5415a834ae570</anchor>
      <arglist>(const LwpaRbTree *self, const LwpaRbNode *a, const LwpaRbNode *b)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rb_tree_node_dealloc_cb</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga3d53f0f67ab15bd6786a403ee15c71a5</anchor>
      <arglist>(const LwpaRbTree *self, LwpaRbNode *node)</arglist>
    </member>
    <member kind="function">
      <type>LwpaRbNode *</type>
      <name>rb_node_init</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gada83215755aa518f6bf3d166a5ff7779</anchor>
      <arglist>(LwpaRbNode *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>LwpaRbTree *</type>
      <name>rb_tree_init</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gab5a8c9076e06fd05f042b25a6d385f02</anchor>
      <arglist>(LwpaRbTree *self, rb_tree_node_cmp_f cmp, rb_node_alloc_f alloc_f, rb_node_dealloc_f dealloc_f)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>rb_tree_find</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gae7649c69dac063892143049d14b78021</anchor>
      <arglist>(LwpaRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rb_tree_insert</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga1d0ad25bb2702bd4d5bd568775474fea</anchor>
      <arglist>(LwpaRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rb_tree_remove</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gad6533c429550b824318c8a654e8e992f</anchor>
      <arglist>(LwpaRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rb_tree_clear</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga3c386b632e95e457a4536385c93e72a0</anchor>
      <arglist>(LwpaRbTree *self)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>rb_tree_size</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga7329ab2313e8d6ac2cffdef6506192c4</anchor>
      <arglist>(LwpaRbTree *self)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rb_tree_insert_node</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga448db091fd66028c31f2507f1dfee961</anchor>
      <arglist>(LwpaRbTree *self, LwpaRbNode *node)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rb_tree_remove_with_cb</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga332ebc443ba728868c326dd4616ad7e9</anchor>
      <arglist>(LwpaRbTree *self, void *value, rb_tree_node_f node_cb)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rb_tree_clear_with_cb</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gae72ff458135e96562d19f412552b0023</anchor>
      <arglist>(LwpaRbTree *self, rb_tree_node_f node_cb)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rb_tree_test</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga2648ae99f6997c45d42a1d4896c8f0cf</anchor>
      <arglist>(LwpaRbTree *self, LwpaRbNode *root)</arglist>
    </member>
    <member kind="function">
      <type>LwpaRbIter *</type>
      <name>rb_iter_init</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga26612f21d99cfdbc6719a96e82f369b8</anchor>
      <arglist>(LwpaRbIter *self)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>rb_iter_first</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga938ea056d3e6fb8b89515f78cf5afbce</anchor>
      <arglist>(LwpaRbIter *self, LwpaRbTree *tree)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>rb_iter_last</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gad75cc5bf03d1455046c47b17acf27dbb</anchor>
      <arglist>(LwpaRbIter *self, LwpaRbTree *tree)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>rb_iter_next</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga627846455078b0b6cdd99fdfe9b6c24d</anchor>
      <arglist>(LwpaRbIter *self)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>rb_iter_prev</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga7cb1cd4f04ffc6d9d14b74bc81c476f0</anchor>
      <arglist>(LwpaRbIter *self)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>rb_tree_node_cmp_f</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gab784fd5c55bf752e7b940d155d434367</anchor>
      <arglist>)(const LwpaRbTree *self, const LwpaRbNode *node_a, const LwpaRbNode *node_b)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>rb_tree_node_f</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gacc50d641f7962fb9f40dc56ae5254e9a</anchor>
      <arglist>)(const LwpaRbTree *self, LwpaRbNode *node)</arglist>
    </member>
    <member kind="typedef">
      <type>LwpaRbNode *(*</type>
      <name>rb_node_alloc_f</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga78d8bafef37c68be88c8a382b5fb024d</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>rb_node_dealloc_f</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gaf6fa55ab036a41506969c3b87f18e02e</anchor>
      <arglist>)(LwpaRbNode *node)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_rootlayerpdu</name>
    <title>lwpa_rootlayerpdu</title>
    <filename>group__lwpa__rootlayerpdu.html</filename>
    <class kind="struct">TcpPreamble</class>
    <class kind="struct">UdpPreamble</class>
    <class kind="struct">RootLayerPdu</class>
    <member kind="define">
      <type>#define</type>
      <name>ACN_TCP_PREAMBLE_SIZE</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gab879b13847509e3bbd4b12a5e2cda0e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_UDP_PREAMBLE_SIZE</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga86207e41512354a1a6e321d95afe8ab0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RLP_HEADER_SIZE_NORMAL_LEN</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga592d004ced57212084bb1760304ea7cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RLP_HEADER_SIZE_EXT_LEN</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga3b0720c08ac5b029b98f25a74906787a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct TcpPreamble</type>
      <name>TcpPreamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga6fc6111bc540bdb2413a9f98bbc95ca4</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct UdpPreamble</type>
      <name>UdpPreamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga78f6296e83e0f028566c0a5ede73188d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct RootLayerPdu</type>
      <name>RootLayerPdu</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gab1eb2da0d3045217e117851856c6f8a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>parse_tcp_preamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga5504243cf58e317bc0f61ccb3191d80d</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, TcpPreamble *preamble)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>parse_udp_preamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gae0be0e22c075acfbbea4330cf1bec23c</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, UdpPreamble *preamble)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>parse_root_layer_header</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gac6f5d91d9ae4549dc6af9ab2aa59b7e8</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, RootLayerPdu *pdu, RootLayerPdu *last_pdu)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>parse_root_layer_pdu</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga16a05260696331878fe93a454dee8e79</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, RootLayerPdu *pdu, LwpaPdu *last_pdu)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>pack_tcp_preamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga470dcd083be11e88e26381ce4f1a7b17</anchor>
      <arglist>(uint8_t *buf, size_t buflen, size_t rlp_block_len)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>pack_udp_preamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gaa95aeb9aa4ca18f8ed26a6523ea9a2c5</anchor>
      <arglist>(uint8_t *buf, size_t buflen)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>root_layer_buf_size</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gaf778285bf86d4d5e8eaf673492fd06a6</anchor>
      <arglist>(const RootLayerPdu *pdu_block, size_t num_pdus)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>pack_root_layer_header</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga508b6176f6fa96d9a22fad3e87d0379b</anchor>
      <arglist>(uint8_t *buf, size_t buflen, const RootLayerPdu *pdu)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>pack_root_layer_block</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga577897f50c0a7a5a78abe74b07520184</anchor>
      <arglist>(uint8_t *buf, size_t buflen, const RootLayerPdu *pdu_block, size_t num_pdus)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_socket</name>
    <title>lwpa_socket</title>
    <filename>group__lwpa__socket.html</filename>
    <class kind="struct">LwpaLinger</class>
    <class kind="struct">LwpaMreq</class>
    <class kind="struct">LwpaPollfd</class>
    <class kind="struct">LwpaAddrinfo</class>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SOCKET_INVALID</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gabee47ca508b8e9b7b15ae6e7729cc087</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_INET_ADDRSTRLEN</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gac9a7a8dbe7def9af922af1a99b4c6569</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_INET6_ADDRSTRLEN</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga078d3101f67934bce6f13f193a4832ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>UNDEFINED</type>
      <name>lwpa_socket_t</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga8fbf24dfb5d096d9f1f2a09e0753a2a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaLinger</type>
      <name>LwpaLinger</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaf190b253ad46048e80a22b6f081d9270</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaMreq</type>
      <name>LwpaMreq</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaa0cd6bdf1e52526e1462a2e54d2f17ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaPollfd</type>
      <name>LwpaPollfd</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga15e820924d167885000b2e8ac733a3a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaAddrinfo</type>
      <name>LwpaAddrinfo</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga26e3a47bd5083fd59eea2099bad33789</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_socket_init</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga7158c969eed13894a54ebb8784381e1f</anchor>
      <arglist>(void *platform_data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_socket_deinit</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga6a9fbbc255caa2094bded46554aeb8c0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_accept</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gac258071ec7b4f5203764ffba0c0d92a7</anchor>
      <arglist>(lwpa_socket_t id, LwpaSockaddr *address, lwpa_socket_t *conn_sock)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_bind</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga826977bc6733ed0c4f34a5746fa05543</anchor>
      <arglist>(lwpa_socket_t id, const LwpaSockaddr *address)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_close</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gab2a3f2980a490bb439f200921074d01f</anchor>
      <arglist>(lwpa_socket_t id)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_connect</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga7ce51adfd80ae9625f3c6c32a6dc66f4</anchor>
      <arglist>(lwpa_socket_t id, const LwpaSockaddr *address)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_getpeername</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga601cc36855c662b79f662ae5c42595f1</anchor>
      <arglist>(lwpa_socket_t id, LwpaSockaddr *address)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_getsockname</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga1268a36d23336543beba21b086af9f4e</anchor>
      <arglist>(lwpa_socket_t id, LwpaSockaddr *address)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_getsockopt</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga76b8fb450bb999d48e6a21f132d75e03</anchor>
      <arglist>(lwpa_socket_t id, int level, int option_name, void *option_value, size_t *option_len)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_listen</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gab5c6cf7dbbff0ebd338e9ef6de183890</anchor>
      <arglist>(lwpa_socket_t id, int backlog)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_recv</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga8a597ac866a5b3951f3b7543e0f96da5</anchor>
      <arglist>(lwpa_socket_t id, void *buffer, size_t length, int flags)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_recvfrom</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gab170a859b1992216dac42722d5773518</anchor>
      <arglist>(lwpa_socket_t id, void *buffer, size_t length, int flags, LwpaSockaddr *address)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_send</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gae22d1b7fe2534cb4a57991a62397faf6</anchor>
      <arglist>(lwpa_socket_t id, const void *message, size_t length, int flags)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_sendto</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gacab7bbbb36484aa2b574ee23652ddc8b</anchor>
      <arglist>(lwpa_socket_t id, const void *message, size_t length, int flags, const LwpaSockaddr *dest_addr)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_setsockopt</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gac74221647c7888f04191a50557c44ec6</anchor>
      <arglist>(lwpa_socket_t id, int level, int option_name, const void *option_value, size_t option_len)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_shutdown</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gae9d6a2363a94d98ee50cdfe359f3715e</anchor>
      <arglist>(lwpa_socket_t id, int how)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_socket_t</type>
      <name>lwpa_socket</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga10b6686c28d4a8e546462e2981541835</anchor>
      <arglist>(unsigned int family, unsigned int type)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_setblocking</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga846cd48d4882273a10d6f781128f03ea</anchor>
      <arglist>(lwpa_socket_t id, bool blocking)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_poll</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaf1cb343c83189a6d72f1c5cf9d2a2d3d</anchor>
      <arglist>(LwpaPollfd *fds, size_t nfds, int timeout_ms)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_getaddrinfo</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga7c1c76581b63ab7c5b383e25dc200990</anchor>
      <arglist>(const char *hostname, const char *service, const LwpaAddrinfo *hints, LwpaAddrinfo *result)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_nextaddr</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gafe366acfdbaa9d98d9459660798c73fd</anchor>
      <arglist>(LwpaAddrinfo *ai)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_freeaddrinfo</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga85eeba64809a88e6a246e9471d5e2cd6</anchor>
      <arglist>(LwpaAddrinfo *ai)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_inet_ntop</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gac6b5fcff898be3bbf7789a8c480082b1</anchor>
      <arglist>(const LwpaIpAddr *src, char *dest, size_t size)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_inet_pton</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga4110e753475f9505e87b88e0097befb4</anchor>
      <arglist>(lwpa_iptype_t type, const char *src, LwpaIpAddr *dest)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SOL_SOCKET</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga3f1556c87ab3fbc847768f2fdcab4e9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IPPROTO_IP</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga774542431c261471aaab85da5e18d991</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IPPROTO_ICMPV6</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaa5f6bba669bdcebac15dab57b5081628</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IPPROTO_IPV6</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga60e65b01d77733f64ed2e0f119f8181f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IPPROTO_TCP</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga95f2416441dfe283fa14b64baa9a4523</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IPPROTO_UDP</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga8b2221240e9f6dc96f115d7271d4fc5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_BROADCAST</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga8254e203b0290da5bfb8975f670f4507</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_ERROR</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaf732095d54985fabfde60f464aad4db7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_KEEPALIVE</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga5f1f963886bbe0d32700e86f3bf22632</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_LINGER</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga55a81e9ec4a9aba857524b726ec75256</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_RCVBUF</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga4391e79e13fa481add7709ec5b5cf563</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_SNDBUF</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga03acc4ad773d79b5c22644c28d27396f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_RCVTIMEO</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga068077e81f7e9a8613d856b6ca6d9166</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_SNDTIMEO</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga70fe9d6049c2d57443614b8c1cfd84c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_REUSEADDR</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaf90682239698ff4ffa860e2ada12ec28</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_REUSEPORT</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga8d68ac126619d3327e3f6fc893c7e488</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_SO_TYPE</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gacad2c1797aca1d5a3c20c272328789c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_TTL</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga2711703676dc5940b484b4831619c23d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_MULTICAST_IF</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga33867e9f8eeb3cbdd8513483026935f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_MULTICAST_TTL</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga2e184196155e7aba01e9f7bc315003b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_MULTICAST_LOOP</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gad0175e8affc43157d2487763ef6660fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_MCAST_JOIN_GROUP</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gab43a3e2b335d9bd16dd775829d259283</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_MCAST_LEAVE_GROUP</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga30d044b29e2f4a000f282ab64a0d74fb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_timer</name>
    <title>lwpa_timer</title>
    <filename>group__lwpa__timer.html</filename>
    <class kind="struct">LwpaTimer</class>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_timer_start</name>
      <anchorfile>group__lwpa__timer.html</anchorfile>
      <anchor>ga9e74e82337cba365573f1dfc98cbb0b1</anchor>
      <arglist>(lwpatimerptr, intval)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_timer_reset</name>
      <anchorfile>group__lwpa__timer.html</anchorfile>
      <anchor>gae39b1a3c9ca6d1efc5b7feef9505f571</anchor>
      <arglist>(lwpatimerptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_timer_elapsed</name>
      <anchorfile>group__lwpa__timer.html</anchorfile>
      <anchor>ga29e6dbbde02e98837f215dd3ee54b551</anchor>
      <arglist>(lwpatimerptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_timer_isexpired</name>
      <anchorfile>group__lwpa__timer.html</anchorfile>
      <anchor>gaee6a48b36dbac8a15ee44195aca894d0</anchor>
      <arglist>(lwpatimerptr)</arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaTimer</type>
      <name>LwpaTimer</name>
      <anchorfile>group__lwpa__timer.html</anchorfile>
      <anchor>gaf03f5495cd147b90b905a0c1cc52ebd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>lwpa_getms</name>
      <anchorfile>group__lwpa__timer.html</anchorfile>
      <anchor>ga94380c14c93286f6a7f1580fd4907d53</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>lwpa_timer_remaining</name>
      <anchorfile>group__lwpa__timer.html</anchorfile>
      <anchor>ga15345971dcc9b6b5e705abe2ac2f24b0</anchor>
      <arglist>(const LwpaTimer *timer)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_uid</name>
    <title>lwpa_uid</title>
    <filename>group__lwpa__uid.html</filename>
    <class kind="struct">LwpaUid</class>
    <member kind="define">
      <type>#define</type>
      <name>uid_cmp</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga1f0e587d7bc26d1adab103e5ed6db9d7</anchor>
      <arglist>(uidptr1, uidptr2)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>uid_equal</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>gab57e197a69d465b71089c5f36e5a510f</anchor>
      <arglist>(uidptr1, uidptr2)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>init_static_uid</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga51b8dcb487d19cc07d3cd926a017b698</anchor>
      <arglist>(uidptr, manu_val, id_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>init_dynamic_uid</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga7f7bee2337e45eb27c8180e8b1178046</anchor>
      <arglist>(uidptr, manu_val, id_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>uid_is_broadcast</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga2fc7f8e2cea15e4fd1a2e903d0ad23ea</anchor>
      <arglist>(uidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>uid_is_rdmnet_controller_broadcast</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>gaabd65340080b6d7d083b9719b2d9723e</anchor>
      <arglist>(uidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>uid_is_rdmnet_device_broadcast</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga51f99ed4b8f1edddf2068ee8c68e8e11</anchor>
      <arglist>(uidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>uid_is_rdmnet_device_manu_broadcast</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga8ef4f2936b3b944c3cceca1d7ebf1801</anchor>
      <arglist>(uidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>rdmnet_device_broadcast_manu_matches</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>gaa173f304e2e1c469670cef664260f543</anchor>
      <arglist>(uidptr, manu_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>rdmnet_device_broadcast_manu_id</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga98d12a84da72b76c2173a46fdc25027b</anchor>
      <arglist>(uidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>uid_is_dynamic</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga00667ebd9ba8321637134546cfb3b69f</anchor>
      <arglist>(uidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>get_manufacturer_id</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga8b42d44d069f01298595e0bd2b52096f</anchor>
      <arglist>(uidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>get_device_id</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga5826ef4fbeab6938db9c83e396a616aa</anchor>
      <arglist>(uidptr)</arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaUid</type>
      <name>LwpaUid</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga90294a8dfa35f35fa9c2d0e55f55fec9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const LwpaUid</type>
      <name>kBroadcastUid</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga82301f49d95c1b5ab9764f96c4565b86</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const LwpaUid</type>
      <name>kRdmnetControllerBroadcastUid</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga743ec44d275cd95349b12a9e4a593693</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const LwpaUid</type>
      <name>kRdmnetDeviceBroadcastUid</name>
      <anchorfile>group__lwpa__uid.html</anchorfile>
      <anchor>ga9fc69dcd792111501cc919b0a88f71d2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>LightWeight Platform Abstraction</title>
    <filename>index.html</filename>
    <docanchor file="index.html">mainpage</docanchor>
  </compound>
</tagfile>
