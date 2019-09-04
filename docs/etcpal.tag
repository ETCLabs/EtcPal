<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>version.h</name>
    <path>D:/git/ETCLabs/lwpa/include/lwpa/</path>
    <filename>version_8h</filename>
  </compound>
  <compound kind="union">
    <name>LwpaIpAddr::AddrUnion</name>
    <filename>union_lwpa_ip_addr_1_1_addr_union.html</filename>
    <class kind="struct">LwpaIpAddr::AddrUnion::LwpaIpv6Addr</class>
  </compound>
  <compound kind="struct">
    <name>CachedNetintInfo</name>
    <filename>struct_cached_netint_info.html</filename>
  </compound>
  <compound kind="struct">
    <name>DefaultNetint</name>
    <filename>struct_default_netint.html</filename>
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
    <name>LwpaGroupReq</name>
    <filename>struct_lwpa_group_req.html</filename>
    <member kind="variable">
      <type>unsigned int</type>
      <name>ifindex</name>
      <anchorfile>struct_lwpa_group_req.html</anchorfile>
      <anchor>a452f46bf9904a02408a12d9e406b4de7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LwpaIpAddr</type>
      <name>group</name>
      <anchorfile>struct_lwpa_group_req.html</anchorfile>
      <anchor>a9d92abf5e7c4488e5d9dfc5ab9376e89</anchor>
      <arglist></arglist>
    </member>
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
    <name>LwpaIpAddr::AddrUnion::LwpaIpv6Addr</name>
    <filename>struct_lwpa_ip_addr_1_1_addr_union_1_1_lwpa_ipv6_addr.html</filename>
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
    <name>LwpaLogStrings</name>
    <filename>struct_lwpa_log_strings.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>syslog</name>
      <anchorfile>struct_lwpa_log_strings.html</anchorfile>
      <anchor>a5911dbe6a47cb2b9cfbab87a8881b5b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>human_readable</name>
      <anchorfile>struct_lwpa_log_strings.html</anchorfile>
      <anchor>a4ac8b9667178849660fd3b1f99e93366</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>raw</name>
      <anchorfile>struct_lwpa_log_strings.html</anchorfile>
      <anchor>a8129637af8920bc9328bbce2a00bfeb4</anchor>
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
    <name>LwpaModuleInit</name>
    <filename>struct_lwpa_module_init.html</filename>
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
      <type>unsigned int</type>
      <name>index</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>a589d64202487f78e3cc30dd2e04c5201</anchor>
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
      <type>uint8_t</type>
      <name>mac</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>af218cb57bca9340eb78204cb81754bc9</anchor>
      <arglist>[LWPA_NETINTINFO_MAC_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>name</name>
      <anchorfile>struct_lwpa_netint_info.html</anchorfile>
      <anchor>abbe9b559861b88e242f78e5eb27b0fd5</anchor>
      <arglist>[LWPA_NETINTINFO_NAME_LEN]</arglist>
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
    <name>LwpaPollEvent</name>
    <filename>struct_lwpa_poll_event.html</filename>
    <member kind="variable">
      <type>lwpa_socket_t</type>
      <name>socket</name>
      <anchorfile>struct_lwpa_poll_event.html</anchorfile>
      <anchor>a8b54dbd84c8737203abd37d960e60779</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>lwpa_poll_events_t</type>
      <name>events</name>
      <anchorfile>struct_lwpa_poll_event.html</anchorfile>
      <anchor>a10ea004722be835cfea1f0c588d6d39c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>lwpa_error_t</type>
      <name>err</name>
      <anchorfile>struct_lwpa_poll_event.html</anchorfile>
      <anchor>a2d84f39897e3cbaf81be57d00dce398b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>user_data</name>
      <anchorfile>struct_lwpa_poll_event.html</anchorfile>
      <anchor>a0f53d287ac7c064d1a49d4bd93ca1cb9</anchor>
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
      <anchor>aca890e30bedb607e7d2164b568197d55</anchor>
      <arglist>[LWPA_RB_ITER_MAX_HEIGHT]</arglist>
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
      <type>lwpa_rbtree_node_cmp_f</type>
      <name>cmp</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>a73b341fcbb4f0cb5c35ad5437de9da0b</anchor>
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
      <type>lwpa_rbnode_alloc_f</type>
      <name>alloc_f</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>a7a4e85551fa2e1fb00d70745c1fd5a65</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>lwpa_rbnode_dealloc_f</type>
      <name>dealloc_f</name>
      <anchorfile>struct_lwpa_rb_tree.html</anchorfile>
      <anchor>a37ee31aceb2b6af53d21336e97ce6330</anchor>
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
    <name>LwpaRootLayerPdu</name>
    <filename>struct_lwpa_root_layer_pdu.html</filename>
    <member kind="variable">
      <type>LwpaUuid</type>
      <name>sender_cid</name>
      <anchorfile>struct_lwpa_root_layer_pdu.html</anchorfile>
      <anchor>addaf02226137035b7f46186c67b87fe5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>vector</name>
      <anchorfile>struct_lwpa_root_layer_pdu.html</anchorfile>
      <anchor>a513d751c7097b43b3968ac81f3a5715b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pdata</name>
      <anchorfile>struct_lwpa_root_layer_pdu.html</anchorfile>
      <anchor>a2f9eb29aa10e0d64f27f69db1e90bd07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>datalen</name>
      <anchorfile>struct_lwpa_root_layer_pdu.html</anchorfile>
      <anchor>a3d12e161726566d24a1d0cf6c6b416bb</anchor>
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
    <name>LwpaTcpPreamble</name>
    <filename>struct_lwpa_tcp_preamble.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>rlp_block</name>
      <anchorfile>struct_lwpa_tcp_preamble.html</anchorfile>
      <anchor>a6df9d0f27538da177c52996a97e0a1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rlp_block_len</name>
      <anchorfile>struct_lwpa_tcp_preamble.html</anchorfile>
      <anchor>ab27f79ca664937f91e64f868ad233935</anchor>
      <arglist></arglist>
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
      <anchor>a9c15c6e0b10239673fa9d8be6ad08128</anchor>
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
    <name>LwpaUdpPreamble</name>
    <filename>struct_lwpa_udp_preamble.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>rlp_block</name>
      <anchorfile>struct_lwpa_udp_preamble.html</anchorfile>
      <anchor>a6df9d0f27538da177c52996a97e0a1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rlp_block_len</name>
      <anchorfile>struct_lwpa_udp_preamble.html</anchorfile>
      <anchor>ab27f79ca664937f91e64f868ad233935</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LwpaUuid</name>
    <filename>struct_lwpa_uuid.html</filename>
  </compound>
  <compound kind="struct">
    <name>MD5_CTX</name>
    <filename>struct_m_d5___c_t_x.html</filename>
  </compound>
  <compound kind="class">
    <name>lwpa::MutexGuard</name>
    <filename>classlwpa_1_1_mutex_guard.html</filename>
  </compound>
  <compound kind="class">
    <name>lwpa::ReadGuard</name>
    <filename>classlwpa_1_1_read_guard.html</filename>
  </compound>
  <compound kind="class">
    <name>lwpa::WriteGuard</name>
    <filename>classlwpa_1_1_write_guard.html</filename>
  </compound>
  <compound kind="namespace">
    <name>lwpa</name>
    <filename>namespacelwpa.html</filename>
    <class kind="class">lwpa::MutexGuard</class>
    <class kind="class">lwpa::ReadGuard</class>
    <class kind="class">lwpa::WriteGuard</class>
  </compound>
  <compound kind="group">
    <name>lwpa_lock</name>
    <title>lwpa_lock</title>
    <filename>group__lwpa__lock.html</filename>
    <subgroup>lwpa_mutex</subgroup>
    <subgroup>lwpa_signal</subgroup>
    <subgroup>lwpa_rwlock</subgroup>
    <class kind="class">lwpa::MutexGuard</class>
    <class kind="class">lwpa::ReadGuard</class>
    <class kind="class">lwpa::WriteGuard</class>
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
      <anchor>gaddfd883159e43fd194fbf3b355411263</anchor>
      <arglist>(lwpa_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_mutex_try_take</name>
      <anchorfile>group__lwpa__mutex.html</anchorfile>
      <anchor>gadc322cce9079e4f7f7ab9e9e685633b0</anchor>
      <arglist>(lwpa_mutex_t *id)</arglist>
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
      <anchor>gad1e8c49a266c700a182c907ef7aa29bf</anchor>
      <arglist>(lwpa_signal_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_signal_poll</name>
      <anchorfile>group__lwpa__signal.html</anchorfile>
      <anchor>ga704ec739497e224a990037524a9086ab</anchor>
      <arglist>(lwpa_signal_t *id)</arglist>
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
      <anchor>gac4f4763199bad441330867f975316393</anchor>
      <arglist>(lwpa_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_rwlock_try_readlock</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>ga9f6b8671a52e91200447ad2912c1a6a3</anchor>
      <arglist>(lwpa_rwlock_t *id)</arglist>
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
      <anchor>gaba4a0078be86e6fc1408cfbe602c0fba</anchor>
      <arglist>(lwpa_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_rwlock_try_writelock</name>
      <anchorfile>group__lwpa__rwlock.html</anchorfile>
      <anchor>ga7f42819fdf2e713b03bf74325e2823f3</anchor>
      <arglist>(lwpa_rwlock_t *id)</arglist>
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
      <anchor>ga57617a14defb483d477a662044d24f74</anchor>
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
      <name>lwpa_thread_join</name>
      <anchorfile>group__lwpa__thread.html</anchorfile>
      <anchor>gac43cbc0cdfd62e365ad76e1110fe359c</anchor>
      <arglist>(lwpa_thread_t *id)</arglist>
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
    <name>lwpa</name>
    <title>lwpa</title>
    <filename>group__lwpa.html</filename>
    <subgroup>lwpa_lock</subgroup>
    <subgroup>lwpa_thread</subgroup>
    <subgroup>lwpa_bool</subgroup>
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
    <subgroup>lwpa_uuid</subgroup>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_WAIT_FOREVER</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>ga1c2b0e972201228ff6c55879bab82861</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>lwpa_features_t</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>gad114d8d0d8fc2b500bbe5cd815f050c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_init</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>gad3f4f0f94eeaede46d9f5f089129446b</anchor>
      <arglist>(lwpa_features_t features)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_deinit</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>gaa5af7daa1612ac3fc98f49a166a0c669</anchor>
      <arglist>(lwpa_features_t features)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_FEATURE_SOCKETS</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>ga51e7968caa7cc29fad18ec872c739cbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_FEATURE_NETINTS</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>ga4830c5a79855564b312b2c12e869702a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_FEATURE_TIMERS</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>ga14efb0458ae41274fe96f0797d35e1f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_FEATURE_LOGGING</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>ga20bf4148474b69f45813f818e16ea0d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_FEATURES_ALL</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>ga1c807b4b9009a27514f7895791ecf49a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_FEATURES_ALL_BUT</name>
      <anchorfile>group__lwpa.html</anchorfile>
      <anchor>ga1effd03384104d4edff4ea16a1c26c9b</anchor>
      <arglist>(mask)</arglist>
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
      <name>kLwpaErrOk</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa332c55060c2e36ba406b247364342199</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNotFound</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa41ff891710b19b8613b5db809a124cf4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNoMem</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa8f782e7c6ea448431c50f088147c5c43</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrBusy</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa3a8c0926d569091c97b9a74fdb716645</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrExists</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa1af6ff7dc223fe0e0bd551fdabd73b7a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrInvalid</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa219aa02b173fd0673404cc488c159e82</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrWouldBlock</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa52905f0701acbbda2f1774e5c3f02069</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNoData</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa5459cfcb7b0a1139177495e00f094b23</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrProtocol</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa9f78328f06545761648fe2d0c9f4036b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrMsgSize</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfadb89f147a1e1e1d28a25c0491acbdf45</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrAddrInUse</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfad0dc4f564ab6a3ae12627d5f727c0361</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrAddrNotAvail</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfafb1d97753ff7441c4525c9696b685196</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNetwork</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa9713abfbdabf5e4ec4d47bf983a9c58b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrConnReset</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa065fca71d26b2f1c18928057c683975e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrConnClosed</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfaeec1e1871e67d57971ec53ab9f0256ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrIsConn</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa77ecfdaace5406dc651a1854b5550799</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNotConn</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfae2395fe58a3c8e5b2a9961acfb3bc382</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrShutdown</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfaa207564d0e57b7d7a0a3b8d42ab67321</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrTimedOut</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa5411409d0e04a1291da3130d90c54a66</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrConnAborted</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa6d3e2a994acb0c378ce59660db281084</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrConnRefused</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfae077aadad03eec8faad0c0898ee467ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrAlready</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa39b9636ec0941f04c09909fd762abf04</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrInProgress</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfabd9b5ff345fe83ebaa753e15f1615649</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrBufSize</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa7397e19c5dc23238112fdcb70610bc71</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNotInit</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa8800538947262274425437ff11e84b25</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNoNetints</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa2c6066ceb92f5c9ca1ad8aa83694d88c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNoSockets</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfad52bd3d08c7896fc789446dfb635243f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrNotImpl</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa88c03c75f09818b154ac60720977adcf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrPerm</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfab2b2faed88fad885e984309f9c6423b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaErrSys</name>
      <anchorfile>group__lwpa__error.html</anchorfile>
      <anchor>gga4de447127cf0baaa9afb0188e1e6afbfa43124f55ae7c2e0f869a49f1fc6e21c9</anchor>
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
      <name>LWPA_IPV6_BYTES</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gadf510df24e827cbeeeca96a00d6cdd75</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_INET_ADDRSTRLEN</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gac9a7a8dbe7def9af922af1a99b4c6569</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_INET6_ADDRSTRLEN</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga078d3101f67934bce6f13f193a4832ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaIpAddr</type>
      <name>LwpaIpAddr</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga42d5bf98514c41052f6f19d2e0a8691e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaSockaddr</type>
      <name>LwpaSockaddr</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gae0b2fa6bb6c8a15a8ebb0cd5fffe96c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaNetintInfo</type>
      <name>LwpaNetintInfo</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga329f214bf9d65ca173437e227a1c5bc7</anchor>
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
      <name>kLwpaIpTypeInvalid</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gga75918e41a21a5b4511b2b3c797e6f048aa8b31e7862dcc08b71037384bb33945e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaIpTypeV4</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gga75918e41a21a5b4511b2b3c797e6f048a73563b977f0118c6094e193f57cfc2cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kLwpaIpTypeV6</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gga75918e41a21a5b4511b2b3c797e6f048a60f28e3cdc84c51debfd05e05b4578da</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_ip_is_link_local</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gace949c44b216e2dd8d81de6b57aafb13</anchor>
      <arglist>(const LwpaIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_ip_is_loopback</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga700cb0526cde413cca61f970231d4806</anchor>
      <arglist>(const LwpaIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_ip_is_multicast</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga3b0cdd32380a0dd44d67fef6800ba95f</anchor>
      <arglist>(const LwpaIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_ip_is_wildcard</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gaaad2d470c8be885c55f8eaf4c89710eb</anchor>
      <arglist>(const LwpaIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_ip_set_wildcard</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga8f5ba7636a445882d3bd09e45f97c285</anchor>
      <arglist>(lwpa_iptype_t type, LwpaIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_ip_equal</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gac1805623603318386ea74815b694f021</anchor>
      <arglist>(const LwpaIpAddr *ip1, const LwpaIpAddr *ip2)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_ip_cmp</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga3aba9897afc056a10b81d4f2d6451d0f</anchor>
      <arglist>(const LwpaIpAddr *ip1, const LwpaIpAddr *ip2)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_ip_and_port_equal</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gad78b7fcae748568c74ecb2c08bfbace4</anchor>
      <arglist>(const LwpaSockaddr *sock1, const LwpaSockaddr *sock2)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>lwpa_ip_mask_length</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga16facd74441deb055bfffaf7c8d7f7ee</anchor>
      <arglist>(const LwpaIpAddr *netmask)</arglist>
    </member>
    <member kind="function">
      <type>LwpaIpAddr</type>
      <name>lwpa_ip_mask_from_length</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga9ef3d4b831b3c9e598f9e568fd17ffe3</anchor>
      <arglist>(lwpa_iptype_t type, unsigned int mask_length)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_ip_network_portions_equal</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga5c416c166428a88fc03f4be742ed6e78</anchor>
      <arglist>(const LwpaIpAddr *ip1, const LwpaIpAddr *ip2, const LwpaIpAddr *netmask)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_IS_V4</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga0b1e21bccece957bb0a4986c51fd7d3a</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_IS_V6</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga6e008633fe046017e20ad036175400a4</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_IS_INVALID</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gae45f4d73820e647d45f4a6c1346fcb0f</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_V4_ADDRESS</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gab58ee5657f5ec2e0a33fe6108d15acfc</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_V6_ADDRESS</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga505ee023d06f2a90976dd50b596ba963</anchor>
      <arglist>(lwpa_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_SET_V4_ADDRESS</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga7a29e94ac67df250504e56ac9257a8c8</anchor>
      <arglist>(lwpa_ip_ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_SET_V6_ADDRESS</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga4217ea63c7520ea2fe349be430c8cfec</anchor>
      <arglist>(lwpa_ip_ptr, addr_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_SET_V6_ADDRESS_WITH_SCOPE_ID</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>ga2d1a4c527a80069754dbc3f08b66723b</anchor>
      <arglist>(lwpa_ip_ptr, addr_val, scope_id_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_SET_INVALID</name>
      <anchorfile>group__lwpa__inet.html</anchorfile>
      <anchor>gaf8587cfa1476afaae1c34081f2b9e061</anchor>
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
    <class kind="struct">LwpaLogStrings</class>
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
      <anchor>gad274aa6b5c42d1ff594200232ccbc0ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaLogStrings</type>
      <name>LwpaLogStrings</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga25c13b78ecd42cad3de3341f66199974</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>lwpa_log_callback</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>gab5ffbb4d8e2168a0c51b10e5b3099de8</anchor>
      <arglist>)(void *context, const LwpaLogStrings *strings)</arglist>
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
      <anchor>ga718e6c57e891ccc8b7e46f15cc164a57</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaLogParams</type>
      <name>LwpaLogParams</name>
      <anchorfile>group__lwpa__log.html</anchorfile>
      <anchor>ga4e65f5bb723d9280bfece5770a224f2c</anchor>
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
      <name>LWPA_MEMPOOL_DEFINE_ARRAY</name>
      <anchorfile>group__lwpa__mempool.html</anchorfile>
      <anchor>ga40f10bc28b7493a35a86c1cb4c1fd766</anchor>
      <arglist>(name, type, array_size, pool_size)</arglist>
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
      <anchor>gabafec40001d48ebd0f6ebfe9fff572d9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_netint</name>
    <title>lwpa_netint</title>
    <filename>group__lwpa__netint.html</filename>
    <member kind="function">
      <type>size_t</type>
      <name>lwpa_netint_get_num_interfaces</name>
      <anchorfile>group__lwpa__netint.html</anchorfile>
      <anchor>gaf5826dbe7cab3ace937a77ca32ecb362</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>lwpa_netint_get_interfaces</name>
      <anchorfile>group__lwpa__netint.html</anchorfile>
      <anchor>ga3e8baf49586da56197583bce01aaaf95</anchor>
      <arglist>(LwpaNetintInfo *netint_arr, size_t netint_arr_size)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_netint_get_default_interface</name>
      <anchorfile>group__lwpa__netint.html</anchorfile>
      <anchor>ga1ef8afff7ab8a6c8f36c4201139afa75</anchor>
      <arglist>(lwpa_iptype_t type, LwpaNetintInfo *netint)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_netint_get_interface_for_dest</name>
      <anchorfile>group__lwpa__netint.html</anchorfile>
      <anchor>ga00eec072ad464f8c19835154ee683cf6</anchor>
      <arglist>(const LwpaIpAddr *dest, LwpaNetintInfo *netint)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_pack</name>
    <title>lwpa_pack</title>
    <filename>group__lwpa__pack.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_upack_16b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gaaaf27b0d45b7d9fdae52a4218774ec35</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pack_16b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga0ff3f85ee64ed231d89baaa57bd13dad</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_upack_16l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gae7c40446cec9afd5a353956fb19eb2fe</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pack_16l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gac839a3e63e3a5d22fbb31c1328927d21</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_upack_32b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>gaf1954a62e2548d796642ed114db84d5c</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pack_32b</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga178005bbc344ef67ee778b6617752a85</anchor>
      <arglist>(ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_upack_32l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga752fcacb7a3d901a169174414408b618</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pack_32l</name>
      <anchorfile>group__lwpa__pack.html</anchorfile>
      <anchor>ga4293fdfec8c32a7296fca6f3ac28dc91</anchor>
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
      <name>lwpa_pdu_length</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gadaf3eef17f0630b17283c1e7d5dd6104</anchor>
      <arglist>(pdu_buf)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_pack_normal_len</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga93536e776d086b3382f063c6ecd2c39c</anchor>
      <arglist>(pdu_buf, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_pack_ext_len</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga38732658618dbe739d909c9cdc71d010</anchor>
      <arglist>(pdu_buf, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_PDU_INIT</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga1b9332f5d2c84f7679f6d37d7abc9abc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_init_pdu</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gabb885bd8514cdb920910fdc815514e2a</anchor>
      <arglist>(pduptr)</arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaPdu</type>
      <name>LwpaPdu</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga5a3510706c6ae05236e3ecd19d8a678d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaPduConstraints</type>
      <name>LwpaPduConstraints</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga2d5812dccfd313f11566fa7fceb5feea</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_parse_pdu</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gacc313a157e7e2b45c67858452512f6c0</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, const LwpaPduConstraints *constraints, LwpaPdu *pdu)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_l_flag_set</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gae5b364603403da45cb766ce7b6511deb</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_v_flag_set</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga9b9015abbec81a0ce77a9458f81a36ad</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_h_flag_set</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga33fb4996b8195d16ef3d067a0325728d</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_d_flag_set</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga27a123d2b8de5379a9fb58bfa530cfd6</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_set_l_flag</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gadb9eaf73cb157935232cf5b918bc7de9</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_set_v_flag</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gaf5c1d329d7d9784de1ac81950d1aece5</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_set_h_flag</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>gab1ac6bc0f38adfb6bd122df4ad896de5</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_pdu_set_d_flag</name>
      <anchorfile>group__lwpa__pdu.html</anchorfile>
      <anchor>ga996c5a9416c5c3a17ded724e9e69ad18</anchor>
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
      <name>LWPA_RB_ITER_MAX_HEIGHT</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gaa24d5568b0d6e32ebf26911d1d0ab31a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaRbIter</type>
      <name>LwpaRbIter</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gaf6c8c65eb7d2d3ffd78c2d692ccfd9f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_rbtree_node_cmp_ptr_cb</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga73fa4fa4c22fb8c6fb9a07235035bfcb</anchor>
      <arglist>(const LwpaRbTree *self, const LwpaRbNode *a, const LwpaRbNode *b)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_rbtree_node_dealloc_cb</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga7ce658fe81d10f1d6fb5e2b2eb431885</anchor>
      <arglist>(const LwpaRbTree *self, LwpaRbNode *node)</arglist>
    </member>
    <member kind="function">
      <type>LwpaRbNode *</type>
      <name>lwpa_rbnode_init</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga1ab2558f54d713524267465e09c68759</anchor>
      <arglist>(LwpaRbNode *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>LwpaRbTree *</type>
      <name>lwpa_rbtree_init</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga0672dba7fbba4e6d1f6d7132bff40478</anchor>
      <arglist>(LwpaRbTree *self, lwpa_rbtree_node_cmp_f cmp, lwpa_rbnode_alloc_f alloc_f, lwpa_rbnode_dealloc_f dealloc_f)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>lwpa_rbtree_find</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga900128f36dacd99eb073a0b5839c6dd8</anchor>
      <arglist>(LwpaRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_rbtree_insert</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gace26e2ddd0124db8ed9d8fa6addc9099</anchor>
      <arglist>(LwpaRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_rbtree_remove</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gadd7548756edcae39dec8c9ca9a24f32e</anchor>
      <arglist>(LwpaRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_rbtree_clear</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gaa26f51f6939eff6a7c5bf5aebc064006</anchor>
      <arglist>(LwpaRbTree *self)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>lwpa_rbtree_size</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga4134368aed440effbdc9c00db986bda6</anchor>
      <arglist>(LwpaRbTree *self)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_rbtree_insert_node</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gab0ae8911b04de195b3e1dfc45b96f4c4</anchor>
      <arglist>(LwpaRbTree *self, LwpaRbNode *node)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_rbtree_remove_with_cb</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga996e8bddf815b66a84fd8cf157520ed1</anchor>
      <arglist>(LwpaRbTree *self, void *value, lwpa_rbtree_node_f node_cb)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_rbtree_clear_with_cb</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga51c0b8012054964179660d156c1f7ccc</anchor>
      <arglist>(LwpaRbTree *self, lwpa_rbtree_node_f node_cb)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>lwpa_rbtree_test</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga58bc2511d88a45dc21f686a6ecb12326</anchor>
      <arglist>(LwpaRbTree *self, LwpaRbNode *root)</arglist>
    </member>
    <member kind="function">
      <type>LwpaRbIter *</type>
      <name>lwpa_rbiter_init</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gad9bc2263f7d1d6d8123b2bd5f4819f37</anchor>
      <arglist>(LwpaRbIter *self)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>lwpa_rbiter_first</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga2b42b70a765f47531dd9fa29ecba0fc8</anchor>
      <arglist>(LwpaRbIter *self, LwpaRbTree *tree)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>lwpa_rbiter_last</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga9ecf1b3abcc0fe0a093d849fa98d3c1a</anchor>
      <arglist>(LwpaRbIter *self, LwpaRbTree *tree)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>lwpa_rbiter_next</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga36bdc60c9ae6b2ac95afa10c9f21d766</anchor>
      <arglist>(LwpaRbIter *self)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>lwpa_rbiter_prev</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gae6350ed5b8ba7a39476bf7e3d582d5c1</anchor>
      <arglist>(LwpaRbIter *self)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>lwpa_rbtree_node_cmp_f</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga5bf67caf3af12bd3dfd679a9886190cd</anchor>
      <arglist>)(const LwpaRbTree *self, const LwpaRbNode *node_a, const LwpaRbNode *node_b)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>lwpa_rbtree_node_f</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga0e25492a71fdf4f04c1970df5224aaa9</anchor>
      <arglist>)(const LwpaRbTree *self, LwpaRbNode *node)</arglist>
    </member>
    <member kind="typedef">
      <type>LwpaRbNode *(*</type>
      <name>lwpa_rbnode_alloc_f</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>gac9b02f8c0316ca42a5da18ff908319d6</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>lwpa_rbnode_dealloc_f</name>
      <anchorfile>group__lwpa__rbtree.html</anchorfile>
      <anchor>ga8bd35866203b0c357afa7969474bbdcd</anchor>
      <arglist>)(LwpaRbNode *node)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_rootlayerpdu</name>
    <title>lwpa_rootlayerpdu</title>
    <filename>group__lwpa__rootlayerpdu.html</filename>
    <class kind="struct">LwpaTcpPreamble</class>
    <class kind="struct">LwpaUdpPreamble</class>
    <class kind="struct">LwpaRootLayerPdu</class>
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
      <name>ACN_RLP_HEADER_SIZE_NORMAL_LEN</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga29ae7e116cd77cc00366461415bf54a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_RLP_HEADER_SIZE_EXT_LEN</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga9d9a7bd4498a28d5b0de8ed598141c66</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaTcpPreamble</type>
      <name>LwpaTcpPreamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga07007c5d21574fd00d811c65f3503acb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaUdpPreamble</type>
      <name>LwpaUdpPreamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga5721d124b0a48d536543434c40e6fe60</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaRootLayerPdu</type>
      <name>LwpaRootLayerPdu</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga1f9d4d412ce818c9a96790ed912c5cc9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_parse_tcp_preamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga349964e773a8e7ecc255cf9a6386f9e8</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, LwpaTcpPreamble *preamble)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_parse_udp_preamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga7cdabf9c84ccb70db5be32237b5387b7</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, LwpaUdpPreamble *preamble)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_parse_root_layer_header</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga43752f0807b577d98c74f1217ec6508f</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, LwpaRootLayerPdu *pdu, LwpaRootLayerPdu *last_pdu)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_parse_root_layer_pdu</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga803a00da49042e85f9b6948ce2cee909</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, LwpaRootLayerPdu *pdu, LwpaPdu *last_pdu)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>lwpa_pack_tcp_preamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga2ead8338b16270f2c835db088786c227</anchor>
      <arglist>(uint8_t *buf, size_t buflen, size_t rlp_block_len)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>lwpa_pack_udp_preamble</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gacaae6efceceeb2a2d5850b60b1dd06f8</anchor>
      <arglist>(uint8_t *buf, size_t buflen)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>lwpa_root_layer_buf_size</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gae1223dfe5d9ebbe99f457a46582e7178</anchor>
      <arglist>(const LwpaRootLayerPdu *pdu_block, size_t num_pdus)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>lwpa_pack_root_layer_header</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>gaffc1246bd07dc0e4a884e94f558eae34</anchor>
      <arglist>(uint8_t *buf, size_t buflen, const LwpaRootLayerPdu *pdu)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>lwpa_pack_root_layer_block</name>
      <anchorfile>group__lwpa__rootlayerpdu.html</anchorfile>
      <anchor>ga75b8ed10289f7085dddb00c44f0bd132</anchor>
      <arglist>(uint8_t *buf, size_t buflen, const LwpaRootLayerPdu *pdu_block, size_t num_pdus)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>lwpa_socket</name>
    <title>lwpa_socket</title>
    <filename>group__lwpa__socket.html</filename>
    <class kind="struct">LwpaLinger</class>
    <class kind="struct">LwpaMreq</class>
    <class kind="struct">LwpaGroupReq</class>
    <class kind="struct">LwpaPollEvent</class>
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
      <name>LWPA_SOCKET_MAX_POLL_SIZE</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gab36e47297a2a48f28c080608c5643c25</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>lwpa_socket_t</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga0fea2946114e0a2617a086c2139fb9f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>lwpa_poll_events_t</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaed05e6a48385343f315c4f1899530a8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaLinger</type>
      <name>LwpaLinger</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga9976c8ca91c28be5e163945e1e1df0eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaMreq</type>
      <name>LwpaMreq</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaf4b1e36ceb83d78b0e4aa513d9fc7af1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaGroupReq</type>
      <name>LwpaGroupReq</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaf497966f431d877d7219574b42e94f4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaPollEvent</type>
      <name>LwpaPollEvent</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga01f60de71f46a394a6421a702475fe2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaAddrinfo</type>
      <name>LwpaAddrinfo</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga10d0c83dc2a02b3b4fec265f769c4b13</anchor>
      <arglist></arglist>
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
      <type>lwpa_error_t</type>
      <name>lwpa_socket</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga00c14a0982e98d8bf1c1aaf8ceaeb64d</anchor>
      <arglist>(unsigned int family, unsigned int type, lwpa_socket_t *id)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_setblocking</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga846cd48d4882273a10d6f781128f03ea</anchor>
      <arglist>(lwpa_socket_t id, bool blocking)</arglist>
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
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>LwpaPollContext</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga78118eb52c46ee1c731ac66c443a4690</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_poll_context_init</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gac75213ffc31b40209d7190dc2237e37a</anchor>
      <arglist>(LwpaPollContext *context)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_poll_context_deinit</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga3f5214a5a60d0727cd87509331ac31c7</anchor>
      <arglist>(LwpaPollContext *context)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_poll_add_socket</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga4659720223c84abac2541a05b11eb371</anchor>
      <arglist>(LwpaPollContext *context, lwpa_socket_t socket, lwpa_poll_events_t events, void *user_data)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_poll_modify_socket</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga1961577a68ff72d7d5924928202712fc</anchor>
      <arglist>(LwpaPollContext *context, lwpa_socket_t socket, lwpa_poll_events_t new_events, void *new_user_data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_poll_remove_socket</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaccbbb8445bb7a6c467807e4f7b8d2605</anchor>
      <arglist>(LwpaPollContext *context, lwpa_socket_t socket)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_poll_wait</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga24182f58155e7894da11e4f621ad42c8</anchor>
      <arglist>(LwpaPollContext *context, LwpaPollEvent *event, int timeout_ms)</arglist>
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
      <name>LWPA_IP_ADD_MEMBERSHIP</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga5c54cc7bb91aa5d1d06491b74515fb45</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IP_DROP_MEMBERSHIP</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gaffd798a26996201a08ae7489f69adb57</anchor>
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
    <member kind="define">
      <type>#define</type>
      <name>LWPA_IPV6_V6ONLY</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gad688a623a81aca19cbba492bf263da69</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_POLL_IN</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gacb4d3326a52b6206680a0974175e28fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_POLL_OUT</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>gac556bc598d758c14d2caa9008314b6d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_POLL_CONNECT</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga29f8b3e0d9b10946134619cc07a42431</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_POLL_OOB</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga9d90f591ffb8078fa05691fa34beb9f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_POLL_ERR</name>
      <anchorfile>group__lwpa__socket.html</anchorfile>
      <anchor>ga86b657b569c5d559cca87ee04773a3a4</anchor>
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
      <anchor>ga609d679562ce78f14e508a6914abbf3a</anchor>
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
    <name>lwpa_uuid</name>
    <title>lwpa_uuid</title>
    <filename>group__lwpa__uuid.html</filename>
    <class kind="struct">LwpaUuid</class>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_UUID_BYTES</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>ga8c2c0266da7a921d450de0c97ba380ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_uuid_cmp</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>ga0a76d940787138f930fc119abbee791d</anchor>
      <arglist>(uuid1ptr, uuid2ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>lwpa_uuid_is_null</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>ga11fe0eb25265c769b4a0abb729b41523</anchor>
      <arglist>(uuidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LWPA_UUID_STRING_BYTES</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>ga1eaab61ad5262363f3ff5739c27f8be5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct LwpaUuid</type>
      <name>LwpaUuid</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>ga176bfe391ccacc68bd4c7c4e01b93384</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lwpa_uuid_to_string</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>ga913c775ec9337470909a993287f9a7e0</anchor>
      <arglist>(char *buf, const LwpaUuid *uuid)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lwpa_string_to_uuid</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>gac0dfc2593ae51c5fc3dbc28d73ce1bd1</anchor>
      <arglist>(LwpaUuid *uuid, const char *buf, size_t buflen)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_generate_v1_uuid</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>ga98aaae9b8d23b7a57fac6f1974ed8bd0</anchor>
      <arglist>(LwpaUuid *uuid)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_generate_v3_uuid</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>ga19d9fbaf6f594ac9ba7e576eaea91201</anchor>
      <arglist>(LwpaUuid *uuid, const char *devstr, const uint8_t *macaddr, uint32_t uuidnum)</arglist>
    </member>
    <member kind="function">
      <type>lwpa_error_t</type>
      <name>lwpa_generate_v4_uuid</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>gad5a92c6e824c30a75fb7f5abda8dec7f</anchor>
      <arglist>(LwpaUuid *uuid)</arglist>
    </member>
    <member kind="variable">
      <type>const LwpaUuid</type>
      <name>kLwpaNullUuid</name>
      <anchorfile>group__lwpa__uuid.html</anchorfile>
      <anchor>gab75cca7a42a1cdc1bb901ed24fc095ee</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>additional_docs</name>
    <title>Additional Documentation</title>
    <filename>additional_docs</filename>
  </compound>
  <compound kind="page">
    <name>interface_indexes</name>
    <title>Network Interface Indexes</title>
    <filename>interface_indexes</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>LightWeight Platform Abstraction</title>
    <filename>index</filename>
  </compound>
</tagfile>
