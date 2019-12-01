<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>error.h</name>
    <path>C:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2error_8h</filename>
    <class kind="class">etcpal::Result</class>
    <class kind="class">etcpal::BadExpectedAccess</class>
    <class kind="class">etcpal::Expected</class>
  </compound>
  <compound kind="file">
    <name>inet.h</name>
    <path>C:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2inet_8h</filename>
    <class kind="class">etcpal::IpAddr</class>
    <class kind="class">etcpal::SockAddr</class>
    <class kind="class">etcpal::MacAddr</class>
    <member kind="enumeration">
      <type></type>
      <name>IpAddrType</name>
      <anchorfile>group__etcpal__cpp__inet.html</anchorfile>
      <anchor>gaf149bcc08653396944a3699b8c5a57c5</anchor>
      <arglist></arglist>
      <enumvalue file="group__etcpal__cpp__inet.html" anchor="ggaf149bcc08653396944a3699b8c5a57c5a4bbb8f967da6d1a610596d7257179c2b">Invalid</enumvalue>
      <enumvalue file="group__etcpal__cpp__inet.html" anchor="ggaf149bcc08653396944a3699b8c5a57c5ab354cab57768c7d4c1106eb4e82c75dc">V4</enumvalue>
      <enumvalue file="group__etcpal__cpp__inet.html" anchor="ggaf149bcc08653396944a3699b8c5a57c5adb93f1c35b844334e2b266951618b4fe">V6</enumvalue>
    </member>
  </compound>
  <compound kind="file">
    <name>lock.h</name>
    <path>C:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2lock_8h</filename>
    <class kind="class">etcpal::Mutex</class>
    <class kind="class">etcpal::Signal</class>
    <class kind="class">etcpal::RwLock</class>
    <class kind="class">etcpal::MutexGuard</class>
    <class kind="class">etcpal::ReadGuard</class>
    <class kind="class">etcpal::WriteGuard</class>
  </compound>
  <compound kind="file">
    <name>uuid.h</name>
    <path>C:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2uuid_8h</filename>
    <class kind="class">etcpal::Uuid</class>
  </compound>
  <compound kind="file">
    <name>version.h</name>
    <path>C:/git/ETCLabs/EtcPal/include/etcpal/</path>
    <filename>version_8h</filename>
  </compound>
  <compound kind="class">
    <name>etcpal::BadExpectedAccess</name>
    <filename>classetcpal_1_1_bad_expected_access.html</filename>
    <member kind="function">
      <type></type>
      <name>BadExpectedAccess</name>
      <anchorfile>classetcpal_1_1_bad_expected_access.html</anchorfile>
      <anchor>aeaa52e8c1a8d52deb6030d756f8e8933</anchor>
      <arglist>(Result res) noexcept</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual const char *</type>
      <name>what</name>
      <anchorfile>classetcpal_1_1_bad_expected_access.html</anchorfile>
      <anchor>abf843cbb29dec939d0731e491bab6f70</anchor>
      <arglist>() const noexcept override</arglist>
    </member>
    <member kind="function">
      <type>Result</type>
      <name>result</name>
      <anchorfile>classetcpal_1_1_bad_expected_access.html</anchorfile>
      <anchor>a099c8aae8a56b9f1278b39c45ddb2269</anchor>
      <arglist>() const noexcept</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalAddrinfo</name>
    <filename>struct_etc_pal_addrinfo.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>ai_flags</name>
      <anchorfile>struct_etc_pal_addrinfo.html</anchorfile>
      <anchor>a92528a848ebf34ab99687dd06a09cc93</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>ai_family</name>
      <anchorfile>struct_etc_pal_addrinfo.html</anchorfile>
      <anchor>a4dc44d22f13bc5c59bff73e549e96a5c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>ai_socktype</name>
      <anchorfile>struct_etc_pal_addrinfo.html</anchorfile>
      <anchor>a2109130e73586150c41fed16311e1af6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>ai_protocol</name>
      <anchorfile>struct_etc_pal_addrinfo.html</anchorfile>
      <anchor>a6ade486587feaa03d492eb84cfc83451</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>ai_canonname</name>
      <anchorfile>struct_etc_pal_addrinfo.html</anchorfile>
      <anchor>a9fcbdd64fcb9adcf195a78b10f1fe8f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalSockAddr</type>
      <name>ai_addr</name>
      <anchorfile>struct_etc_pal_addrinfo.html</anchorfile>
      <anchor>a2000f4ebfae33b230309b0344354031d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pd</name>
      <anchorfile>struct_etc_pal_addrinfo.html</anchorfile>
      <anchor>aecf048e565653706ca7120829d06e047</anchor>
      <arglist>[2]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalGroupReq</name>
    <filename>struct_etc_pal_group_req.html</filename>
    <member kind="variable">
      <type>unsigned int</type>
      <name>ifindex</name>
      <anchorfile>struct_etc_pal_group_req.html</anchorfile>
      <anchor>a452f46bf9904a02408a12d9e406b4de7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalIpAddr</type>
      <name>group</name>
      <anchorfile>struct_etc_pal_group_req.html</anchorfile>
      <anchor>ab8ed4113f331d191260b09e960e94254</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalIpAddr</name>
    <filename>struct_etc_pal_ip_addr.html</filename>
    <member kind="variable">
      <type>etcpal_iptype_t</type>
      <name>type</name>
      <anchorfile>struct_etc_pal_ip_addr.html</anchorfile>
      <anchor>a89368dca599010efedd3d8ca663d5953</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>v4</name>
      <anchorfile>struct_etc_pal_ip_addr.html</anchorfile>
      <anchor>a654069046d83e652d54cae5e353051c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>addr_buf</name>
      <anchorfile>struct_etc_pal_ip_addr.html</anchorfile>
      <anchor>a53d9f270f12d57a90a97d60b5e612237</anchor>
      <arglist>[ETCPAL_IPV6_BYTES]</arglist>
    </member>
    <member kind="variable">
      <type>unsigned long</type>
      <name>scope_id</name>
      <anchorfile>struct_etc_pal_ip_addr.html</anchorfile>
      <anchor>a77c3e8275263425c8924ad010b38fae7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>union EtcPalIpAddr::@0</type>
      <name>addr</name>
      <anchorfile>struct_etc_pal_ip_addr.html</anchorfile>
      <anchor>a3423eb761e78396c67ba1345c0438871</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalLinger</name>
    <filename>struct_etc_pal_linger.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>onoff</name>
      <anchorfile>struct_etc_pal_linger.html</anchorfile>
      <anchor>a7730494d5c90b034d5937c758f354997</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>linger</name>
      <anchorfile>struct_etc_pal_linger.html</anchorfile>
      <anchor>a21667e862207c7c116b20764fcb8f931</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalLogParams</name>
    <filename>struct_etc_pal_log_params.html</filename>
    <member kind="variable">
      <type>etcpal_log_action_t</type>
      <name>action</name>
      <anchorfile>struct_etc_pal_log_params.html</anchorfile>
      <anchor>a97b4c2de584d731ffb8d8ce759930691</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>etcpal_log_callback</type>
      <name>log_fn</name>
      <anchorfile>struct_etc_pal_log_params.html</anchorfile>
      <anchor>a878e8584783aed32b30a35697ffa27b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalSyslogParams</type>
      <name>syslog_params</name>
      <anchorfile>struct_etc_pal_log_params.html</anchorfile>
      <anchor>a6b5edefad9cb5463180d2f3cc9537c44</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>log_mask</name>
      <anchorfile>struct_etc_pal_log_params.html</anchorfile>
      <anchor>ac695d621aaa836a9d13d65a912be9518</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>etcpal_log_time_fn</type>
      <name>time_fn</name>
      <anchorfile>struct_etc_pal_log_params.html</anchorfile>
      <anchor>aaeb7319ba51e6b5fe1f4df5dea2eabce</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>context</name>
      <anchorfile>struct_etc_pal_log_params.html</anchorfile>
      <anchor>ae376f130b17d169ee51be68077a89ed0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalLogStrings</name>
    <filename>struct_etc_pal_log_strings.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>syslog</name>
      <anchorfile>struct_etc_pal_log_strings.html</anchorfile>
      <anchor>a5911dbe6a47cb2b9cfbab87a8881b5b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>human_readable</name>
      <anchorfile>struct_etc_pal_log_strings.html</anchorfile>
      <anchor>a4ac8b9667178849660fd3b1f99e93366</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>raw</name>
      <anchorfile>struct_etc_pal_log_strings.html</anchorfile>
      <anchor>a8129637af8920bc9328bbce2a00bfeb4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalLogTimeParams</name>
    <filename>struct_etc_pal_log_time_params.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>year</name>
      <anchorfile>struct_etc_pal_log_time_params.html</anchorfile>
      <anchor>abeac221e38b7b9ce7df8722c842bf671</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>month</name>
      <anchorfile>struct_etc_pal_log_time_params.html</anchorfile>
      <anchor>aedb06abe5aff12fa3e7e0e71a374edfb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>day</name>
      <anchorfile>struct_etc_pal_log_time_params.html</anchorfile>
      <anchor>a4c11afc03fc3ee49bab660def6558f2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>hour</name>
      <anchorfile>struct_etc_pal_log_time_params.html</anchorfile>
      <anchor>a15df9ba285cfd842f284025f904edc9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>minute</name>
      <anchorfile>struct_etc_pal_log_time_params.html</anchorfile>
      <anchor>a5edffad982a0566ad01d95005474eae3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>second</name>
      <anchorfile>struct_etc_pal_log_time_params.html</anchorfile>
      <anchor>a6cf35be1947a62f134392fcb1b3c54d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>msec</name>
      <anchorfile>struct_etc_pal_log_time_params.html</anchorfile>
      <anchor>ad957f34cd6e8272740bc231865080b22</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>utc_offset</name>
      <anchorfile>struct_etc_pal_log_time_params.html</anchorfile>
      <anchor>a0d5ceb6295477b5931bf54c002e36fe9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalMacAddr</name>
    <filename>struct_etc_pal_mac_addr.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>data</name>
      <anchorfile>struct_etc_pal_mac_addr.html</anchorfile>
      <anchor>a251236fb1e367947c8f6f5498a86896b</anchor>
      <arglist>[ETCPAL_MAC_BYTES]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalMempoolDesc</name>
    <filename>struct_etc_pal_mempool_desc.html</filename>
    <member kind="variable">
      <type>const size_t</type>
      <name>elem_size</name>
      <anchorfile>struct_etc_pal_mempool_desc.html</anchorfile>
      <anchor>a817b1b9f628c2698a9a3fb16ce738530</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const size_t</type>
      <name>pool_size</name>
      <anchorfile>struct_etc_pal_mempool_desc.html</anchorfile>
      <anchor>aaeb9b2d37542fba1b318e4fcde49d9b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalMempool *</type>
      <name>freelist</name>
      <anchorfile>struct_etc_pal_mempool_desc.html</anchorfile>
      <anchor>a6e77cf602471114d51631a441c42ab0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalMempool *const</type>
      <name>list</name>
      <anchorfile>struct_etc_pal_mempool_desc.html</anchorfile>
      <anchor>a9bbae4d416583392b93135d8cc44d80e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>current_used</name>
      <anchorfile>struct_etc_pal_mempool_desc.html</anchorfile>
      <anchor>aff331a9d3e0fdee5a3a504a3fe685bf0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *const</type>
      <name>pool</name>
      <anchorfile>struct_etc_pal_mempool_desc.html</anchorfile>
      <anchor>aece14e36f8731b6f1c9602fee75c1544</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalMreq</name>
    <filename>struct_etc_pal_mreq.html</filename>
    <member kind="variable">
      <type>EtcPalIpAddr</type>
      <name>netint</name>
      <anchorfile>struct_etc_pal_mreq.html</anchorfile>
      <anchor>af4d9906a2a7fca6dfe6daf61b1ac55d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalIpAddr</type>
      <name>group</name>
      <anchorfile>struct_etc_pal_mreq.html</anchorfile>
      <anchor>ab8ed4113f331d191260b09e960e94254</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalNetintInfo</name>
    <filename>struct_etc_pal_netint_info.html</filename>
    <member kind="variable">
      <type>unsigned int</type>
      <name>index</name>
      <anchorfile>struct_etc_pal_netint_info.html</anchorfile>
      <anchor>a589d64202487f78e3cc30dd2e04c5201</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalIpAddr</type>
      <name>addr</name>
      <anchorfile>struct_etc_pal_netint_info.html</anchorfile>
      <anchor>ac10a4235b585003054f76871bad3fd85</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalIpAddr</type>
      <name>mask</name>
      <anchorfile>struct_etc_pal_netint_info.html</anchorfile>
      <anchor>a155577b467420f0877aa19b7265565f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalMacAddr</type>
      <name>mac</name>
      <anchorfile>struct_etc_pal_netint_info.html</anchorfile>
      <anchor>a166afed9bf79aa905a870e4a130923ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>name</name>
      <anchorfile>struct_etc_pal_netint_info.html</anchorfile>
      <anchor>a6eaa1f18016764aa8812196745e36463</anchor>
      <arglist>[ETCPAL_NETINTINFO_NAME_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>friendly_name</name>
      <anchorfile>struct_etc_pal_netint_info.html</anchorfile>
      <anchor>a3fa69022d71df27910811117d3e232fd</anchor>
      <arglist>[ETCPAL_NETINTINFO_FRIENDLY_NAME_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>is_default</name>
      <anchorfile>struct_etc_pal_netint_info.html</anchorfile>
      <anchor>a0c9f19a7b68ad0dad836ee153d3a230a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalPdu</name>
    <filename>struct_etc_pal_pdu.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pvector</name>
      <anchorfile>struct_etc_pal_pdu.html</anchorfile>
      <anchor>a106ad8e138ad9d47e5ed03680c6ac6ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pheader</name>
      <anchorfile>struct_etc_pal_pdu.html</anchorfile>
      <anchor>a89e577801ef19d4cdca129391395ad8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pdata</name>
      <anchorfile>struct_etc_pal_pdu.html</anchorfile>
      <anchor>a2f9eb29aa10e0d64f27f69db1e90bd07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>datalen</name>
      <anchorfile>struct_etc_pal_pdu.html</anchorfile>
      <anchor>a3d12e161726566d24a1d0cf6c6b416bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pnextpdu</name>
      <anchorfile>struct_etc_pal_pdu.html</anchorfile>
      <anchor>a5af45b626c13232f152d7548f29f9f58</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalPduConstraints</name>
    <filename>struct_etc_pal_pdu_constraints.html</filename>
    <member kind="variable">
      <type>size_t</type>
      <name>vector_size</name>
      <anchorfile>struct_etc_pal_pdu_constraints.html</anchorfile>
      <anchor>a28e18243c88fd99888246e855cc116fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>header_size</name>
      <anchorfile>struct_etc_pal_pdu_constraints.html</anchorfile>
      <anchor>a64f1d0c53909f30b0aac882348cd32f2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalPollEvent</name>
    <filename>struct_etc_pal_poll_event.html</filename>
    <member kind="variable">
      <type>etcpal_socket_t</type>
      <name>socket</name>
      <anchorfile>struct_etc_pal_poll_event.html</anchorfile>
      <anchor>ad32899ff82c07979513229c6f83c5f05</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>etcpal_poll_events_t</type>
      <name>events</name>
      <anchorfile>struct_etc_pal_poll_event.html</anchorfile>
      <anchor>a8c1a86a9a616989de3fcced4b0c0e1f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>etcpal_error_t</type>
      <name>err</name>
      <anchorfile>struct_etc_pal_poll_event.html</anchorfile>
      <anchor>a13b5fda068afcd9c79a54263a9fe7b07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>user_data</name>
      <anchorfile>struct_etc_pal_poll_event.html</anchorfile>
      <anchor>a0f53d287ac7c064d1a49d4bd93ca1cb9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalRbIter</name>
    <filename>struct_etc_pal_rb_iter.html</filename>
    <member kind="variable">
      <type>EtcPalRbTree *</type>
      <name>tree</name>
      <anchorfile>struct_etc_pal_rb_iter.html</anchorfile>
      <anchor>a3dc7beb088200d742f70c0c895b376c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalRbNode *</type>
      <name>node</name>
      <anchorfile>struct_etc_pal_rb_iter.html</anchorfile>
      <anchor>a46745baa1082347489f1dd6dd68c7afa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalRbNode *</type>
      <name>path</name>
      <anchorfile>struct_etc_pal_rb_iter.html</anchorfile>
      <anchor>a6d248403d379988dac19bf98cfeb84b9</anchor>
      <arglist>[ETCPAL_RB_ITER_MAX_HEIGHT]</arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>top</name>
      <anchorfile>struct_etc_pal_rb_iter.html</anchorfile>
      <anchor>ab9332a87d0af5aa12cac847eff68b7af</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>info</name>
      <anchorfile>struct_etc_pal_rb_iter.html</anchorfile>
      <anchor>acb1df3a0f703b05bc4971f79cabe2597</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalRbNode</name>
    <filename>struct_etc_pal_rb_node.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>red</name>
      <anchorfile>struct_etc_pal_rb_node.html</anchorfile>
      <anchor>a6761340706096510fd89edca40a63b9b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalRbNode *</type>
      <name>link</name>
      <anchorfile>struct_etc_pal_rb_node.html</anchorfile>
      <anchor>a629e7a1728cfe5d93fe2381d793ef93f</anchor>
      <arglist>[2]</arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>value</name>
      <anchorfile>struct_etc_pal_rb_node.html</anchorfile>
      <anchor>a0f61d63b009d0880a89c843bd50d8d76</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalRbTree</name>
    <filename>struct_etc_pal_rb_tree.html</filename>
    <member kind="variable">
      <type>EtcPalRbNode *</type>
      <name>root</name>
      <anchorfile>struct_etc_pal_rb_tree.html</anchorfile>
      <anchor>ae96f5aa846cba3ae538255b01f066bea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalRbTreeNodeCmpFunc</type>
      <name>cmp</name>
      <anchorfile>struct_etc_pal_rb_tree.html</anchorfile>
      <anchor>a7b29df8f3907a7cb322be7a1cb130670</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>size</name>
      <anchorfile>struct_etc_pal_rb_tree.html</anchorfile>
      <anchor>a854352f53b148adc24983a58a1866d66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalRbNodeAllocFunc</type>
      <name>alloc_f</name>
      <anchorfile>struct_etc_pal_rb_tree.html</anchorfile>
      <anchor>a2969aaea1ac7bf7f69dba4965e8b8d5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalRbNodeDeallocFunc</type>
      <name>dealloc_f</name>
      <anchorfile>struct_etc_pal_rb_tree.html</anchorfile>
      <anchor>a29c69388460d3c40b35a388dcbea6b58</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>info</name>
      <anchorfile>struct_etc_pal_rb_tree.html</anchorfile>
      <anchor>acb1df3a0f703b05bc4971f79cabe2597</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalRootLayerPdu</name>
    <filename>struct_etc_pal_root_layer_pdu.html</filename>
    <member kind="variable">
      <type>EtcPalUuid</type>
      <name>sender_cid</name>
      <anchorfile>struct_etc_pal_root_layer_pdu.html</anchorfile>
      <anchor>a341064da4798492e212c14a9aeede58d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>vector</name>
      <anchorfile>struct_etc_pal_root_layer_pdu.html</anchorfile>
      <anchor>a513d751c7097b43b3968ac81f3a5715b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pdata</name>
      <anchorfile>struct_etc_pal_root_layer_pdu.html</anchorfile>
      <anchor>a2f9eb29aa10e0d64f27f69db1e90bd07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>datalen</name>
      <anchorfile>struct_etc_pal_root_layer_pdu.html</anchorfile>
      <anchor>a3d12e161726566d24a1d0cf6c6b416bb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalSockAddr</name>
    <filename>struct_etc_pal_sock_addr.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>port</name>
      <anchorfile>struct_etc_pal_sock_addr.html</anchorfile>
      <anchor>a8e0798404bf2cf5dabb84c5ba9a4f236</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EtcPalIpAddr</type>
      <name>ip</name>
      <anchorfile>struct_etc_pal_sock_addr.html</anchorfile>
      <anchor>a19cc8016f534a565f984438cfe911a9e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalSyslogParams</name>
    <filename>struct_etc_pal_syslog_params.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>facility</name>
      <anchorfile>struct_etc_pal_syslog_params.html</anchorfile>
      <anchor>a213afa2e73e0c2102f43d4067e41b90f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>hostname</name>
      <anchorfile>struct_etc_pal_syslog_params.html</anchorfile>
      <anchor>acf45ed2a37e15e83167bd1edcd9fbbbe</anchor>
      <arglist>[ETCPAL_LOG_HOSTNAME_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>app_name</name>
      <anchorfile>struct_etc_pal_syslog_params.html</anchorfile>
      <anchor>a6d20b2dcf8c2bc4f864b3d719ad00ad0</anchor>
      <arglist>[ETCPAL_LOG_APP_NAME_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>procid</name>
      <anchorfile>struct_etc_pal_syslog_params.html</anchorfile>
      <anchor>aa3d06040894562832d2218511875b444</anchor>
      <arglist>[ETCPAL_LOG_PROCID_MAX_LEN]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalTcpPreamble</name>
    <filename>struct_etc_pal_tcp_preamble.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>rlp_block</name>
      <anchorfile>struct_etc_pal_tcp_preamble.html</anchorfile>
      <anchor>a6df9d0f27538da177c52996a97e0a1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rlp_block_len</name>
      <anchorfile>struct_etc_pal_tcp_preamble.html</anchorfile>
      <anchor>ab27f79ca664937f91e64f868ad233935</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalThreadParams</name>
    <filename>struct_etc_pal_thread_params.html</filename>
    <member kind="variable">
      <type>unsigned int</type>
      <name>thread_priority</name>
      <anchorfile>struct_etc_pal_thread_params.html</anchorfile>
      <anchor>a9cc3e9ae3970c47e150dc77065abd75f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned int</type>
      <name>stack_size</name>
      <anchorfile>struct_etc_pal_thread_params.html</anchorfile>
      <anchor>adde5266300e9cdd7ca1134daba9adf24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>thread_name</name>
      <anchorfile>struct_etc_pal_thread_params.html</anchorfile>
      <anchor>af081a07b0c5074db7c1896da8fb72e81</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>platform_data</name>
      <anchorfile>struct_etc_pal_thread_params.html</anchorfile>
      <anchor>ac7426f48be885f66ad98c90f342f653b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalTimer</name>
    <filename>struct_etc_pal_timer.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>reset_time</name>
      <anchorfile>struct_etc_pal_timer.html</anchorfile>
      <anchor>a8fb66f63094794fbbdacc69af5cc55fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>interval</name>
      <anchorfile>struct_etc_pal_timer.html</anchorfile>
      <anchor>acd9ed1ccc44de55e1d3eff5569663695</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalUdpPreamble</name>
    <filename>struct_etc_pal_udp_preamble.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>rlp_block</name>
      <anchorfile>struct_etc_pal_udp_preamble.html</anchorfile>
      <anchor>a6df9d0f27538da177c52996a97e0a1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rlp_block_len</name>
      <anchorfile>struct_etc_pal_udp_preamble.html</anchorfile>
      <anchor>ab27f79ca664937f91e64f868ad233935</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalUuid</name>
    <filename>struct_etc_pal_uuid.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>data</name>
      <anchorfile>struct_etc_pal_uuid.html</anchorfile>
      <anchor>a814a9e92ac05977a8fce43ba49425d14</anchor>
      <arglist>[ETCPAL_UUID_BYTES]</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::Expected</name>
    <filename>classetcpal_1_1_expected.html</filename>
    <templarg>T</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>ValueType</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>aecaeabaa9da616ae42bb20787878260d</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a05eee239980486bf36c4f8950d967b3d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>afbcd61d5831db1cae7c98f61c6b487f7</anchor>
      <arglist>(const Expected &amp;other)=default</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ad1e0dc7fef59a1d8b87d22cd586cfcf7</anchor>
      <arglist>(Expected &amp;&amp;other)=default</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a9218bac8e2e2f3ad495bf4c9f0a6973f</anchor>
      <arglist>(const Expected&lt; U &gt; &amp;other, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, const U &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; const U &amp;, T &gt;::value))</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a22a38e2d07ffb1e9295aa2237ada9fc6</anchor>
      <arglist>(const Expected&lt; U &gt; &amp;other, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, const U &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;&amp;, T &gt;::value &amp;&amp;std::is_convertible&lt; const U &amp;, T &gt;::value))</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>aa3dc47e18317ee5dfe579355d6a43dfe</anchor>
      <arglist>(Expected&lt; U &gt; &amp;&amp;other, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, U &gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; U, T &gt;::value))</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ab140b28680238373a39161bed5cd002a</anchor>
      <arglist>(Expected&lt; U &gt; &amp;&amp;other, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, U &gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;&amp;, T &gt;::value &amp;&amp;std::is_convertible&lt; U, T &gt;::value))</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a59c942fe4fb741cdd75818afe46aa58e</anchor>
      <arglist>(U &amp;&amp;value, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, U &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; U &amp;&amp;, T &gt;::value)) noexcept(std::is_nothrow_move_constructible&lt; U &gt;::value)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a30568dad48509352dedb7ba6a16899b4</anchor>
      <arglist>(U &amp;&amp;value, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, U &amp;&amp;&gt;::value &amp;&amp;std::is_convertible&lt; U &amp;&amp;, T &gt;::value)) noexcept(std::is_nothrow_move_constructible&lt; U &gt;::value)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a372215ac699d087a7413d2bba7b47c1c</anchor>
      <arglist>(etcpal_error_t error)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a8da4910031b4040699233168372b0102</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>constexpr const T *</type>
      <name>operator-&gt;</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a59a8d66ef67871af2a14f8cc08dc1d2e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>T *</type>
      <name>operator-&gt;</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a4442b1b9eab40e7939e1e01cdedcb7ae</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>constexpr const T &amp;</type>
      <name>operator*</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a6573281a3d9396dfc8c2c5f1cca82247</anchor>
      <arglist>() const &amp;</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>operator*</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>aef577cb229ef2a9d16ea231ab32bb90d</anchor>
      <arglist>() &amp;</arglist>
    </member>
    <member kind="function">
      <type>constexpr const T &amp;&amp;</type>
      <name>operator*</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a9fde1d75aef70226df99291f44c4c7d5</anchor>
      <arglist>() const &amp;&amp;</arglist>
    </member>
    <member kind="function">
      <type>constexpr T &amp;&amp;</type>
      <name>operator*</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a4b1b0ac2a151c256b031fef21f6651cc</anchor>
      <arglist>() &amp;&amp;</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>operator bool</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a7990d8e87d9eab2383f94c88c23a6488</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>has_value</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a6988ba57722f67cbb444b0931468282e</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr const T &amp;</type>
      <name>value</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>aa3795c04ad2644e8294f7f028bde5393</anchor>
      <arglist>() const &amp;</arglist>
    </member>
    <member kind="function">
      <type>constexpr T &amp;</type>
      <name>value</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a1db9740321489652d5420f80c57b5643</anchor>
      <arglist>() &amp;</arglist>
    </member>
    <member kind="function">
      <type>constexpr const T &amp;&amp;</type>
      <name>value</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>af0677a7aa98cba5e3a267da236f28175</anchor>
      <arglist>() const &amp;&amp;</arglist>
    </member>
    <member kind="function">
      <type>constexpr T &amp;&amp;</type>
      <name>value</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ab71ecf7b054666c750a48f0249ea7ff1</anchor>
      <arglist>() &amp;&amp;</arglist>
    </member>
    <member kind="function">
      <type>constexpr etcpal_error_t</type>
      <name>error</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a50623f3a6e41cdc3b27d8658808797c8</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr Result</type>
      <name>result</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a28c72c40e1a8ff5c26c86ffd8b3220df</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr T</type>
      <name>value_or</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a6d0e422ee375e26a6b855848674309d5</anchor>
      <arglist>(U &amp;&amp;def_val) const &amp;</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>value_or</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>aa673d764741af614d297d71e984e592f</anchor>
      <arglist>(U &amp;&amp;def_val) &amp;&amp;</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::IpAddr</name>
    <filename>classetcpal_1_1_ip_addr.html</filename>
    <member kind="function">
      <type></type>
      <name>IpAddr</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a4a003a4fea5fb690128013fc94677ff5</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IpAddr</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>aa0ecd73e87259b2e2d5435673c8e5149</anchor>
      <arglist>(const EtcPalIpAddr &amp;c_ip) noexcept</arglist>
    </member>
    <member kind="function">
      <type>IpAddr &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>af68058aa8f42f5ce7aa01eb92b583bd1</anchor>
      <arglist>(const EtcPalIpAddr &amp;c_ip) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IpAddr</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a47a9c1b3e45125fac51fa2d28b19beeb</anchor>
      <arglist>(uint32_t v4_data) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IpAddr</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a10a0e9f0e89374ef5543a40bb6e9aafb</anchor>
      <arglist>(const uint8_t *v6_data) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IpAddr</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>abd1c05fc9b5b627f239972390c0bfeab</anchor>
      <arglist>(const uint8_t *v6_data, unsigned long scope_id) noexcept</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalIpAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a19515352bb017814527c2be05f352a1f</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>EtcPalIpAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a03ca80b4d8a6285595ba334e1e6f3180</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>ToString</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a19c380b03cea21d7ac7325136a131ff0</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>v4_data</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>ac916b557b42f871359ce597c4c17556b</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const uint8_t *</type>
      <name>v6_data</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a092dcb96eb18fa8c6526322cd6d62245</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::array&lt; uint8_t, ETCPAL_IPV6_BYTES &gt;</type>
      <name>ToV6Array</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a9d6d96f2d5b5fb9fd6a16bcceb8dea59</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>unsigned long</type>
      <name>scope_id</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a3ddc22a71f0df9873b7bbf20b7b4a54e</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsValid</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a472f705194218179e9cf26ffb288da6c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>IpAddrType</type>
      <name>type</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a72e87c00e9e7eb9c88fcbd0f030cc59c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsV4</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a77c4dddfd37b29508860336f844cd745</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsV6</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>aca9ab0a7348c500efcfa08168dd70f85</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsLinkLocal</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a211a72caa8aba79efd967fa3114e8c95</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsLoopback</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a4833f2d2e3f0434978f59f5020a7f84b</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsMulticast</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a2f371a6bc900dd3ddcf0f7f16c5db00a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsWildcard</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>ac384adc1cbf470fe6672659bdef8358c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetAddress</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a289b584a63be9da802a0692f16a9a7fc</anchor>
      <arglist>(uint32_t v4_data) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetAddress</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a5b87e44914b8e5d6acc662c0e7ada452</anchor>
      <arglist>(const uint8_t *v6_data) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetAddress</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a5e188695085f61a2fd9c0ba28a6e0893</anchor>
      <arglist>(const uint8_t *v6_data, unsigned long scope_id) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IpAddr</type>
      <name>FromString</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a5c2b23004b9bdca3df3142b5f4fbd8ac</anchor>
      <arglist>(const std::string &amp;ip_str) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IpAddr</type>
      <name>WildcardV4</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a8ddb6547f191f5d3f3b24f3f02727095</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IpAddr</type>
      <name>WildcardV6</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a24fd3077450fceffa3e02fb4c76be7df</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IpAddr</type>
      <name>Wildcard</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a6dd5f9ef79404118ec2427c68eedad86</anchor>
      <arglist>(IpAddrType type) noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::MacAddr</name>
    <filename>classetcpal_1_1_mac_addr.html</filename>
    <member kind="function">
      <type></type>
      <name>MacAddr</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>ae745add18e97ed3960a513f993c4fd39</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MacAddr</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>ae3a2a6ea57714e57bf4c829449e2f04e</anchor>
      <arglist>(const EtcPalMacAddr &amp;c_mac) noexcept</arglist>
    </member>
    <member kind="function">
      <type>MacAddr &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>abf74026b5f8428336b974584cbd534df</anchor>
      <arglist>(const EtcPalMacAddr &amp;c_mac) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MacAddr</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>aff3e9838bb257f6f5c09396be07fa4f3</anchor>
      <arglist>(const uint8_t *mac_data) noexcept</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalMacAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>ad707eabf21debd3afc8a47f28a41c96c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>EtcPalMacAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>abd82397c8172b871348b31a16473f61e</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>ToString</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>a19c380b03cea21d7ac7325136a131ff0</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const uint8_t *</type>
      <name>data</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>a21b8e060baa6eeed3e5e45e6d760b54c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::array&lt; uint8_t, ETCPAL_MAC_BYTES &gt;</type>
      <name>ToArray</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>a9ef1e40476be6ec748b2cb8d1e7235ce</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsNull</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>a27909eb1d61b8b261d259be7c70c58dd</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MacAddr</type>
      <name>FromString</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>a4ddd74c443669ab0d7c1aefac0e13fd7</anchor>
      <arglist>(const std::string &amp;mac_str) noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::Mutex</name>
    <filename>classetcpal_1_1_mutex.html</filename>
    <member kind="function">
      <type></type>
      <name>Mutex</name>
      <anchorfile>classetcpal_1_1_mutex.html</anchorfile>
      <anchor>a4b5dfbd24f5bc4c03b24b2da362f66d5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Mutex</name>
      <anchorfile>classetcpal_1_1_mutex.html</anchorfile>
      <anchor>a205e2c334b25cb96e4f1303a4fde6b0c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>Lock</name>
      <anchorfile>classetcpal_1_1_mutex.html</anchorfile>
      <anchor>a747fd95916e116a6ed37cc641e394c69</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>TryLock</name>
      <anchorfile>classetcpal_1_1_mutex.html</anchorfile>
      <anchor>a0fc3000efb073879e1c437d9e3d4e7f6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Unlock</name>
      <anchorfile>classetcpal_1_1_mutex.html</anchorfile>
      <anchor>a91b88a5d5517cb042431c4ea24d8ecb7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>etcpal_mutex_t &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_mutex.html</anchorfile>
      <anchor>a969165b5fad27bd7ae7a693703124645</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::MutexGuard</name>
    <filename>classetcpal_1_1_mutex_guard.html</filename>
    <member kind="function">
      <type></type>
      <name>MutexGuard</name>
      <anchorfile>classetcpal_1_1_mutex_guard.html</anchorfile>
      <anchor>a72577d0ea01f7607a671de23cc54eb06</anchor>
      <arglist>(Mutex &amp;mutex)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MutexGuard</name>
      <anchorfile>classetcpal_1_1_mutex_guard.html</anchorfile>
      <anchor>a057d36a0206d67c6b8b9c5d8186c2e9e</anchor>
      <arglist>(etcpal_mutex_t &amp;mutex)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MutexGuard</name>
      <anchorfile>classetcpal_1_1_mutex_guard.html</anchorfile>
      <anchor>a310e9063e3e9794762a2cf28c6148334</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::ReadGuard</name>
    <filename>classetcpal_1_1_read_guard.html</filename>
    <member kind="function">
      <type></type>
      <name>ReadGuard</name>
      <anchorfile>classetcpal_1_1_read_guard.html</anchorfile>
      <anchor>a4bbe0587143829b91a5169449360eaa4</anchor>
      <arglist>(RwLock &amp;rwlock)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ReadGuard</name>
      <anchorfile>classetcpal_1_1_read_guard.html</anchorfile>
      <anchor>a972ca1d800d0b9f661de63a8764b74ae</anchor>
      <arglist>(etcpal_rwlock_t &amp;rwlock)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ReadGuard</name>
      <anchorfile>classetcpal_1_1_read_guard.html</anchorfile>
      <anchor>af40fbc4ea83eb91ffa11e2e0a17e5e22</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::Result</name>
    <filename>classetcpal_1_1_result.html</filename>
    <member kind="function">
      <type></type>
      <name>Result</name>
      <anchorfile>classetcpal_1_1_result.html</anchorfile>
      <anchor>a1632987f6b0d6d53fbecd0cc123558f0</anchor>
      <arglist>(etcpal_error_t code) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Result &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_result.html</anchorfile>
      <anchor>aa17ae9a95db6d0224d2280bbb1f8667d</anchor>
      <arglist>(etcpal_error_t code) noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsOk</name>
      <anchorfile>classetcpal_1_1_result.html</anchorfile>
      <anchor>a644febf37130fba98a432cef161bccc0</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>code</name>
      <anchorfile>classetcpal_1_1_result.html</anchorfile>
      <anchor>a3379f90d50bc0858a3c2fb44eb4f5edd</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>ToString</name>
      <anchorfile>classetcpal_1_1_result.html</anchorfile>
      <anchor>a19c380b03cea21d7ac7325136a131ff0</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>ToCString</name>
      <anchorfile>classetcpal_1_1_result.html</anchorfile>
      <anchor>aa4d860f883bc4c6bc997eb61f218a27e</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>operator bool</name>
      <anchorfile>classetcpal_1_1_result.html</anchorfile>
      <anchor>a420e60e9788bfa3b52aa3aab72ea3a5e</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Result</type>
      <name>Ok</name>
      <anchorfile>classetcpal_1_1_result.html</anchorfile>
      <anchor>a9908177b0717b32031b195426058cc9a</anchor>
      <arglist>() noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::RwLock</name>
    <filename>classetcpal_1_1_rw_lock.html</filename>
    <member kind="function">
      <type></type>
      <name>RwLock</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>ad5ce7091375083d76ae798cabac53a21</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~RwLock</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>a31fcfa6d9b8bcf5a64823de7c0b1313a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>ReadLock</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>af760aebc292f4430c5d554b1fb239284</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>TryReadLock</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>a409d488d3b55cffa68f081ee05611764</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ReadUnlock</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>a1860d89c46fea6b62da4c7c39756fc4e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>WriteLock</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>a062052c7ab44662cc7d5a2bf81f4a87c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>TryWriteLock</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>a6c1151df4aab7df16590bee65e504873</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>WriteUnlock</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>abfc3bd44b6252464c577c5ed8389dbd4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>etcpal_rwlock_t &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_rw_lock.html</anchorfile>
      <anchor>a16634332a0b44a7bedfcc491383c4fe5</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::Signal</name>
    <filename>classetcpal_1_1_signal.html</filename>
    <member kind="function">
      <type></type>
      <name>Signal</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>a44146dfdbec17a8ab7207e52ffae152c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Signal</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>a20038cb30942f02ef70185cdaed0227a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>Wait</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>ae5ccc0d2886026ce37e12604173a131d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>Poll</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>a86acd23bf483d12b47cedfe52333e643</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Notify</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>a6b945888ffd146338605d77f0366141b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>etcpal_signal_t &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>a57e433d2fa16d80ff2f02992c74d480c</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::SockAddr</name>
    <filename>classetcpal_1_1_sock_addr.html</filename>
    <member kind="function">
      <type></type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>ab509e15c062994750a41ff72761acab2</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a953bdc1c8acc45e6339bfc809478c395</anchor>
      <arglist>(const EtcPalSockAddr &amp;c_sa) noexcept</arglist>
    </member>
    <member kind="function">
      <type>SockAddr &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>aa75677e1db37e94003d6f82a25934b0f</anchor>
      <arglist>(const EtcPalSockAddr &amp;c_sa) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a18104a287d9daf45ef6058197c80d6b9</anchor>
      <arglist>(uint32_t v4_data, uint16_t port) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a47df50740970dda850f7f8d748381dd4</anchor>
      <arglist>(const uint8_t *v6_data, uint16_t port) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>afc7a705bc0a693bf397d9862d9cd8b0a</anchor>
      <arglist>(const uint8_t *v6_data, unsigned long scope_id, uint16_t port) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a9b2e0399b2484eaea02c6858f98331fc</anchor>
      <arglist>(IpAddr ip, uint16_t port) noexcept</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalSockAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>aafa2e8990aa4c3bcac9d7dc52820ff00</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>EtcPalSockAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>ab6767c93f33490f5fec0303eba1d8772</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>ToString</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a19c380b03cea21d7ac7325136a131ff0</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>IpAddr</type>
      <name>ip</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>acbb9df748594ef90cf431ae61c84f547</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>port</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a6aac8bbc4a1ac6c37d4c9f0564256fbd</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetAddress</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a289b584a63be9da802a0692f16a9a7fc</anchor>
      <arglist>(uint32_t v4_data) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetAddress</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a5b87e44914b8e5d6acc662c0e7ada452</anchor>
      <arglist>(const uint8_t *v6_data) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetAddress</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a5e188695085f61a2fd9c0ba28a6e0893</anchor>
      <arglist>(const uint8_t *v6_data, unsigned long scope_id) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetAddress</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a297607ac00a2f71ea955edfbaa43875f</anchor>
      <arglist>(const IpAddr &amp;ip) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetPort</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a8f2a678aed9c3bdd389633dea859b5d2</anchor>
      <arglist>(uint16_t port) noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::Uuid</name>
    <filename>classetcpal_1_1_uuid.html</filename>
    <member kind="function">
      <type></type>
      <name>Uuid</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>abc4f198636744d10b7c5e28a1ddde4b8</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Uuid</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a4c60e6639ac1d1a2937013511f409198</anchor>
      <arglist>(const EtcPalUuid &amp;c_uuid)</arglist>
    </member>
    <member kind="function">
      <type>Uuid &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a7e35f2362ce4dd50ce9ce390dc28b48e</anchor>
      <arglist>(const EtcPalUuid &amp;c_uuid)</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalUuid &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a6f9078659deb922d762d311a9f558dae</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>ToString</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a19c380b03cea21d7ac7325136a131ff0</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsNull</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>ac4d637a0dec45d546495c1660c9f6047</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>FromString</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a348be5fbf7a863c74485b6df0e029fdc</anchor>
      <arglist>(const std::string &amp;uuid_str)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V1</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a81948e83f4876fd61bb771433ec0cfc3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V3</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a71d95c31678e670a44669651a2527695</anchor>
      <arglist>(const std::string &amp;device_str, const std::array&lt; uint8_t, 6 &gt; &amp;mac_addr, uint32_t uuid_num)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V4</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a6c0e58d9c0173c62f95571e65b2ec484</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>OsPreferred</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a99ed3b8b32434cff4b17cb1acb03c9ce</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::WriteGuard</name>
    <filename>classetcpal_1_1_write_guard.html</filename>
    <member kind="function">
      <type></type>
      <name>WriteGuard</name>
      <anchorfile>classetcpal_1_1_write_guard.html</anchorfile>
      <anchor>a0c3e55817a2955c086af3a033db5105a</anchor>
      <arglist>(RwLock &amp;rwlock)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>WriteGuard</name>
      <anchorfile>classetcpal_1_1_write_guard.html</anchorfile>
      <anchor>a94731b7b53c630d8781f78807ca35dd2</anchor>
      <arglist>(etcpal_rwlock_t &amp;rwlock)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~WriteGuard</name>
      <anchorfile>classetcpal_1_1_write_guard.html</anchorfile>
      <anchor>afa219384bacf93a154fdb6bf71941d57</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_bool</name>
    <title>C Boolean Type (bool)</title>
    <filename>group__etcpal__bool.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>bool</name>
      <anchorfile>group__etcpal__bool.html</anchorfile>
      <anchor>gabb452686968e48b67397da5f97445f5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>false</name>
      <anchorfile>group__etcpal__bool.html</anchorfile>
      <anchor>ga65e9886d74aaee76545e83dd09011727</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>true</name>
      <anchorfile>group__etcpal__bool.html</anchorfile>
      <anchor>ga41f9c5fb8b08eb5dc3edce4dcb37fee7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal</name>
    <title>EtcPal</title>
    <filename>group__etcpal.html</filename>
    <subgroup>etcpal_bool</subgroup>
    <subgroup>etcpal_error</subgroup>
    <subgroup>etcpal_inet</subgroup>
    <subgroup>etcpal_int</subgroup>
    <subgroup>etcpal_log</subgroup>
    <subgroup>etcpal_mempool</subgroup>
    <subgroup>etcpal_netint</subgroup>
    <subgroup>etcpal_pack</subgroup>
    <subgroup>etcpal_pdu</subgroup>
    <subgroup>etcpal_rbtree</subgroup>
    <subgroup>etcpal_socket</subgroup>
    <subgroup>etcpal_thread</subgroup>
    <subgroup>etcpal_timer</subgroup>
    <subgroup>etcpal_uuid</subgroup>
    <subgroup>etcpal_lock</subgroup>
    <subgroup>etcpal_opts</subgroup>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_WAIT_FOREVER</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>ga1cfb33811295439bc08ce2e8b74c1d13</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>etcpal_features_t</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>gae77772998c26f45618c26de1e1fc8305</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_init</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>ga163b95dfe5ef6c68f2fb4d5b58a63843</anchor>
      <arglist>(etcpal_features_t features)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_deinit</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>ga0792bdcd2f824009584744db01d7a352</anchor>
      <arglist>(etcpal_features_t features)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_FEATURE_SOCKETS</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>ga09f99422deaa7bb5d2625c317d1a66a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_FEATURE_NETINTS</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>gaf5b3a81b975bbae1f6f54f43bfc0c6a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_FEATURE_TIMERS</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>gaffb285a8108a672c01c33d921ee8be83</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_FEATURE_LOGGING</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>ga8c7c1f9430362540e56af3a2742a25d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_FEATURES_ALL</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>ga740dd7c89f16ff20992cf21970d1c521</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_FEATURES_ALL_BUT</name>
      <anchorfile>group__etcpal.html</anchorfile>
      <anchor>ga15560671c4e732ed46dc03a0e522d160</anchor>
      <arglist>(mask)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_error</name>
    <title>Error Handling (error)</title>
    <filename>group__etcpal__error.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_NUM_ERROR_CODES</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gae0de193fbaf16861756958bd3f21e07f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>etcpal_error_t</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>ga743b0648f0ca798ef29c9f69c6e255af</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrOk</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afae403d046b4ba4e81dce121266c2cca8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNotFound</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa62c704cfc2d72f1fa91c04efe508c60f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNoMem</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afad16b0fe51e8664f8b30b740ea1e51599</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrBusy</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa47b93a8acf0aad836afc5dda44551acc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrExists</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afaeab0ae4d277d52dc56a1d78f81868c2d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrInvalid</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afadb17bb2ec24e7755170e8afbd5b10bc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrWouldBlock</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa735dc55ef30ad08cca792cb41a1dfeb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNoData</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa77e50551fc1c569578f0f9dd4d4a5712</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrProtocol</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa8396c97054393b3faf3849541fc99c43</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrMsgSize</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afaa980a5e26cf5086f916d12a485572f12</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrAddrInUse</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afae27e1c52937b3d1c1993f78160a0a778</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrAddrNotAvail</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa44ede5b2503ce2b1bb4ccd285d676b50</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNetwork</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa4c458cd2885e27a726a1d9dedf4b4bec</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrConnReset</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa15cf4f4905d98d94910c6cc76ae1218f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrConnClosed</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afafb8046b7504e0e9908654f939e74ee60</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrIsConn</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa9603628c356775915b437e3c15bdef53</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNotConn</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa2eb8abeafb170747d185803ad0a2d527</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrShutdown</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa28e1dbbb288f3248d64378d83920259a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrTimedOut</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afac1a55c311993d815ab367c339f550956</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrHostUnreach</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa9c5af0e9de48f403358d6b9e2ef819b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrConnAborted</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa39f50bbaad22488dba1944f129d1ecde</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrConnRefused</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa4b119ec2231eb8bf73535b1375f5f232</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrAlready</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afaa9c6c39f7303953a99c50a981f36dff0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrInProgress</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa99aabe562db1ed9acab67c6a2bc777a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrBufSize</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afaefe61c95876a3a7faba0f5da4ec03f37</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNotInit</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afadfb4ade13d03ed30f6309d2f130f05f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNoNetints</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa730afdd022ada726bdf8d940eda9759b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNoSockets</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afaa45464bce734de8eca7f5a768386423b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrNotImpl</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afad0b98165ec7fb9169790675a67850d52</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrPerm</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa9fb1e2b91f204d8a01377796bddd867d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalErrSys</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>gga743b0648f0ca798ef29c9f69c6e255afa38404e113508a0e291dbcc6c73743ae8</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>etcpal_strerror</name>
      <anchorfile>group__etcpal__error.html</anchorfile>
      <anchor>ga4902a840d9801e02baebd4c70924c4b0</anchor>
      <arglist>(etcpal_error_t code)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_inet</name>
    <title>Internet Addressing (inet)</title>
    <filename>group__etcpal__inet.html</filename>
    <class kind="struct">EtcPalIpAddr</class>
    <class kind="struct">EtcPalSockAddr</class>
    <class kind="struct">EtcPalMacAddr</class>
    <class kind="struct">EtcPalNetintInfo</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPV6_BYTES</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gad5f18940857b50304b956505fb77df20</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MAC_BYTES</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga2222b6d9e40f95ec1d74e1865537863f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MAC_CMP</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga4aadcedf06a097132f1eef8176b83419</anchor>
      <arglist>(mac1ptr, mac2ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MAC_IS_NULL</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga84ac8ff7c11b7b3d05e66192205ae0cb</anchor>
      <arglist>(macptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_NETINTINFO_NAME_LEN</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga492fc3dd7fabb325e80e375d6be69d89</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_NETINTINFO_FRIENDLY_NAME_LEN</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga7f8d8782f4dea64648bb74e20db6d3c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_INET_ADDRSTRLEN</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gab44293e291772ce05ffa42c1ad51e4e0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_INET6_ADDRSTRLEN</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga71fa8216f8c0c7472c8786e793a7b280</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MAC_STRING_BYTES</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga73f73b31cc309024d3284ebcf4b66b45</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalIpAddr</type>
      <name>EtcPalIpAddr</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga0f4fa2c5534edc2061c48b7d0a60941f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalSockAddr</type>
      <name>EtcPalSockAddr</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga0178b5f6d720a2293ef10bb61dcecf7a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalMacAddr</type>
      <name>EtcPalMacAddr</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gac0911b094d33366046d9b947b24cc63f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalNetintInfo</type>
      <name>EtcPalNetintInfo</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaed988a9502c1d15538ef945b3b5b234b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>etcpal_iptype_t</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga847282e3fd17bf75ec83292326c455ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalIpTypeInvalid</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gga847282e3fd17bf75ec83292326c455eea0e789a4e187f4a7fdbda7271a754eca8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalIpTypeV4</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gga847282e3fd17bf75ec83292326c455eea73ea615eceba61f26d92a63f3738f635</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalIpTypeV6</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gga847282e3fd17bf75ec83292326c455eead480ed8d686b23bdb2e5bfc06b93d7ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_ip_is_link_local</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaa4b21b32f8776276e582d473e17bf768</anchor>
      <arglist>(const EtcPalIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_ip_is_loopback</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga6f59d1ebf7873649f0812ee1cae171e7</anchor>
      <arglist>(const EtcPalIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_ip_is_multicast</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga71b745e5bdc7c77bba5320241639f450</anchor>
      <arglist>(const EtcPalIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_ip_is_wildcard</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaef79c0c7eac819d7decf1473246d3b82</anchor>
      <arglist>(const EtcPalIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_ip_set_wildcard</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga5d91044202026978c99000b6da2d226e</anchor>
      <arglist>(etcpal_iptype_t type, EtcPalIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>etcpal_ip_cmp</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaf766d207bd3e85c6b3d75768f28a00c6</anchor>
      <arglist>(const EtcPalIpAddr *ip1, const EtcPalIpAddr *ip2)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_ip_and_port_equal</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaf897a891bcf59ccb447845628c2a04c2</anchor>
      <arglist>(const EtcPalSockAddr *sock1, const EtcPalSockAddr *sock2)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>etcpal_ip_mask_length</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga9a86a44681bc856e5919578bbf652e38</anchor>
      <arglist>(const EtcPalIpAddr *netmask)</arglist>
    </member>
    <member kind="function">
      <type>EtcPalIpAddr</type>
      <name>etcpal_ip_mask_from_length</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga0f8ba14963ce91bfd74626f2a1e957f0</anchor>
      <arglist>(etcpal_iptype_t type, unsigned int mask_length)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_ip_network_portions_equal</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga2112e37aae54451f5b7b03168183cace</anchor>
      <arglist>(const EtcPalIpAddr *ip1, const EtcPalIpAddr *ip2, const EtcPalIpAddr *netmask)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>ip_os_to_etcpal</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gad3a164fa82579c3047f6053f7266e6a8</anchor>
      <arglist>(const etcpal_os_ipaddr_t *os_ip, EtcPalIpAddr *ip)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>ip_etcpal_to_os</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gafe364c911711c469e0574821e30600e3</anchor>
      <arglist>(const EtcPalIpAddr *ip, etcpal_os_ipaddr_t *os_ip)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>sockaddr_os_to_etcpal</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga087efaef8630f32173eb397b4cb1ae7f</anchor>
      <arglist>(const etcpal_os_sockaddr_t *os_sa, EtcPalSockAddr *sa)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>sockaddr_etcpal_to_os</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaa8d939a802f0ab99ae1a6e8f9253a83d</anchor>
      <arglist>(const EtcPalSockAddr *sa, etcpal_os_sockaddr_t *os_sa)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_inet_ntop</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga8f8b3a64cec296d5d896e10bf7a60ac3</anchor>
      <arglist>(const EtcPalIpAddr *src, char *dest, size_t size)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_inet_pton</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gacc09a02c09d9798f46d1fb6e9e84b900</anchor>
      <arglist>(etcpal_iptype_t type, const char *src, EtcPalIpAddr *dest)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_mac_to_string</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga4d0f54c9182caac7f69ec49116d42448</anchor>
      <arglist>(const EtcPalMacAddr *src, char *dest, size_t size)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_string_to_mac</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gac2175076e5d4834fccf76e17a29bbbfe</anchor>
      <arglist>(const char *src, EtcPalMacAddr *dest)</arglist>
    </member>
    <member kind="variable">
      <type>const EtcPalMacAddr</type>
      <name>kEtcPalNullMacAddr</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gae648df0d068e3335073224fb0fb45512</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_IS_V4</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga4ed3a313f7503031100bb169ad03bc89</anchor>
      <arglist>(etcpal_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_IS_V6</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga85d241eed93be1b271e5dc68a56e9473</anchor>
      <arglist>(etcpal_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_IS_INVALID</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga56a6933874bc52770eac5865f237e225</anchor>
      <arglist>(etcpal_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_V4_ADDRESS</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga35262011f1ae20211d8aad5c79ab1b50</anchor>
      <arglist>(etcpal_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_V6_ADDRESS</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaaee800da95371923f9375dec1a6c0a5c</anchor>
      <arglist>(etcpal_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_V6_SCOPE_ID</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga88982a2d40356f278c53563ddf96d8b2</anchor>
      <arglist>(etcpal_ip_ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_SET_V4_ADDRESS</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga5e36f7ec503aef51829aed6fbd88c889</anchor>
      <arglist>(etcpal_ip_ptr, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_SET_V6_ADDRESS</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gae0d14b2f44b81a6687390839e0aea869</anchor>
      <arglist>(etcpal_ip_ptr, addr_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_SET_V6_ADDRESS_WITH_SCOPE_ID</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaeb96cdc37b6f57f448b0ac15971380d8</anchor>
      <arglist>(etcpal_ip_ptr, addr_val, scope_id_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_SET_INVALID</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gab7df63744affbe2833fcb455e69b6ff5</anchor>
      <arglist>(etcpal_ip_ptr)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_int</name>
    <title>Fixed-Width Integer Types (int)</title>
    <filename>group__etcpal__int.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>HAVE_STDINT_H</name>
      <anchorfile>group__etcpal__int.html</anchorfile>
      <anchor>gab6cd6d1c63c1e26ea2d4537b77148354</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_log</name>
    <title>Logging (log)</title>
    <filename>group__etcpal__log.html</filename>
    <class kind="struct">EtcPalLogTimeParams</class>
    <class kind="struct">EtcPalLogStrings</class>
    <class kind="struct">EtcPalSyslogParams</class>
    <class kind="struct">EtcPalLogParams</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_NFACILITIES</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga9c650b00d427b03b6169b18514627eb7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_FAC</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gab60e596c37c275724a2d27c0ef608197</anchor>
      <arglist>(p)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_PRI</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga62757cfe0dd5a61600cbeed6777480d1</anchor>
      <arglist>(p)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_MAKEPRI</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga443fcc43be128430ebb6d1c82fe6b894</anchor>
      <arglist>(fac, pri)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_MASK</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga153d45bd7f441311296802b70f704700</anchor>
      <arglist>(pri)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_UPTO</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaecd624f2fc15671bb3e4c01ede198a08</anchor>
      <arglist>(pri)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_HOSTNAME_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gafc4fcfbaaffb603d10e2b27f54e122d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_APP_NAME_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga2b336b7e2828f1f9c576fb4a6e357b1a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_PROCID_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga1786d1616917fb0bf1315504e14060c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_MSG_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga2ee8e585233bc73a2a299fc94230fed4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_TIMESTAMP_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga589d05713f16b04a0337399d5064e7c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SYSLOG_HEADER_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga6463250841fd9f0b87d84574d5ed561f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SYSLOG_STR_MIN_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga2b86d5334ad5c012a7b81ce1e9c90e33</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_HUMAN_LOG_STR_MIN_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gac2b8a36af28b78d0060a1d89b9b18211</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SYSLOG_STR_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gae8f0782821bb95f1c2a0122b9819acac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_HUMAN_LOG_STR_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga0fafd8567d72170461c34b0f4b4cde74</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalLogTimeParams</type>
      <name>EtcPalLogTimeParams</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga3cfc242617ad25826b445fce64b800a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalLogStrings</type>
      <name>EtcPalLogStrings</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaf51673a8ceea21c025198456ac795a12</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>etcpal_log_callback</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga2149ec5b6a6311efd2c235358f3c49c7</anchor>
      <arglist>)(void *context, const EtcPalLogStrings *strings)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>etcpal_log_time_fn</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga738d510e7988bf2364ed059fb193a5ae</anchor>
      <arglist>)(void *context, EtcPalLogTimeParams *time_params)</arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalSyslogParams</type>
      <name>EtcPalSyslogParams</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga3961fedc222b8d388ebcb2bbe6e00263</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalLogParams</type>
      <name>EtcPalLogParams</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gac6a69c52ee1d8dea2aa9d6a90d217050</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>etcpal_log_action_t</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga10855f3c1f3816d0bd5e4687108ea1c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalLogCreateSyslog</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gga10855f3c1f3816d0bd5e4687108ea1c5a1631725f7a5f65a9fb15767cecec5290</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalLogCreateHumanReadableLog</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gga10855f3c1f3816d0bd5e4687108ea1c5ae82eb0e5fc80d2657c033413df36224e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>kEtcPalLogCreateBoth</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gga10855f3c1f3816d0bd5e4687108ea1c5a8d083dc7d2d4359fd9ce6330b9d4c2bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_create_syslog_str</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga066f11ddd10e75d38a379a648d2dcba4</anchor>
      <arglist>(char *buf, size_t buflen, const EtcPalLogTimeParams *time, const EtcPalSyslogParams *syslog_params, int pri, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_create_human_log_str</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaeb1f0786f9b553d05991a97db73e3dad</anchor>
      <arglist>(char *buf, size_t buflen, const EtcPalLogTimeParams *time, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_sanitize_syslog_params</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga895ff8dd5230b10b4ab6d955e556d318</anchor>
      <arglist>(EtcPalSyslogParams *params)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_validate_log_params</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga949455773a4ecdd6a499940a19ec6080</anchor>
      <arglist>(EtcPalLogParams *params)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_can_log</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gac44c39be61d1bf95239f9bd6fb1f11ad</anchor>
      <arglist>(const EtcPalLogParams *params, int pri)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_log</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gab05a59d52b560561ce33a937f73aae5e</anchor>
      <arglist>(const EtcPalLogParams *params, int pri, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_vlog</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga2f8f5602ed9ad8a869d40c4af8ba8d41</anchor>
      <arglist>(const EtcPalLogParams *params, int pri, const char *format, va_list args)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_KERN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga6ae744f52d1ee08daa8fd36bf58597b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_USER</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga6fc21fb437c4fa1d90dd8698cfa256f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_MAIL</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga42fd837e598b56123c5d38fff2f5fbeb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_DAEMON</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga525978d1bfe16fcd0a4450a7c5dc53ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_AUTH</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gabc74c1a4cef75ebe20e917b68fa23d52</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_SYSLOG</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga46bba655f9c87face73f5e2e680f0e6c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LPR</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga185481461d7f07edfd95fa80405b37c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_NEWS</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga83c2035b9784a9cb0625bc4f33fbdc76</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_UUCP</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaaa5608f22b53b0de9b6c4b1811d632ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_CRON</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga076221d5039086994c7b2a87963be388</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_AUTHPRIV</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga0c3bcdd91c9d747dea59430a849556b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_FTP</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaa3ad1dbc725353f2cd49cc55b32f3153</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LOCAL0</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga64040cea6d8c1457ef47e65279f8bca2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LOCAL1</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga2f6c98d265250c28e6102c97a10dee17</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LOCAL2</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga752539f8bfef40f0c19dc4ef67a59473</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LOCAL3</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga92640deb4a6a2625e4fc36038cd8d45d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LOCAL4</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga25a6a69270ce194f38d3d24e17c1d830</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LOCAL5</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gad7d901793d960e58420eb4025cfa51d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LOCAL6</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gabcdbdf6cbaeec1b06545372144162a79</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_LOCAL7</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga3b702549e92e23e2918d2fae9af6a84c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_EMERG</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaaba03ada613553b9276df87d7387ae6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_ALERT</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaefbc8eee2db20c20586eaae217faa385</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_CRIT</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga1f5ee8379832669d8cb7d5abc2e93b1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_ERR</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga0f39e099c1569ddf03e826eb4e831cf1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_WARNING</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gac79a4a2a830ae8db541f33093b2d50df</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_NOTICE</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga2d71e55473353be02c7fd2a785b12b23</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_INFO</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga9f05a0ad034946dfd20477e11d2923a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_LOG_DEBUG</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga4e46d489885b63c38263e35ad5aca8b6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_mempool</name>
    <title>Memory Pools (mempool)</title>
    <filename>group__etcpal__mempool.html</filename>
    <class kind="struct">EtcPalMempoolDesc</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MEMPOOL_DECLARE</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>ga1da98d192da467dfb2ec751bad279178</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MEMPOOL_DEFINE</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>ga0d56307540e32e74ce5a87a52ce447b8</anchor>
      <arglist>(name, type, size)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MEMPOOL_DEFINE_ARRAY</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>ga3a974aa750cca64861b630f0a7776232</anchor>
      <arglist>(name, type, array_size, pool_size)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>etcpal_mempool_init</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>ga2b7b0144747ea853ea526f283344c047</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>etcpal_mempool_alloc</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>ga3cda8bdb4dc13e12ba5e75ccadb23274</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>etcpal_mempool_free</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>gaf69c62d9c8df5f589fefbaf0283b2435</anchor>
      <arglist>(name, mem)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>etcpal_mempool_size</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>ga8b10935dcb7665a6c4418831fd17c9cd</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>etcpal_mempool_used</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>gaa1a695b25f4ea895750e753303eb4f85</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalMempoolDesc</type>
      <name>EtcPalMempoolDesc</name>
      <anchorfile>group__etcpal__mempool.html</anchorfile>
      <anchor>ga4b65b2221f81b94eba323dc3bd827cd4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_netint</name>
    <title>Network Interfaces (netint)</title>
    <filename>group__etcpal__netint.html</filename>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_netint_get_num_interfaces</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>ga14c0e51d047bc84b5c4f783c640fc030</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalNetintInfo *</type>
      <name>etcpal_netint_get_interfaces</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>gafc68a9f3fc1342e3d747ab0b17029ee5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_netint_copy_interfaces</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>ga2b88beeb498044aa1c390638346422b4</anchor>
      <arglist>(EtcPalNetintInfo *netint_arr, size_t netint_arr_size)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_netint_get_interfaces_by_index</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>ga78f5df18395e14e959ed0c4dc26580dc</anchor>
      <arglist>(unsigned int index, const EtcPalNetintInfo **netint_arr, size_t *netint_arr_size)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_netint_get_default_interface</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>gae5cfdf3d1aa1edf3b6f3729491241911</anchor>
      <arglist>(etcpal_iptype_t type, unsigned int *netint_index)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_netint_get_interface_for_dest</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>gae0297aaa85a237ffd910644cb414750a</anchor>
      <arglist>(const EtcPalIpAddr *dest, unsigned int *netint_index)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_pack</name>
    <title>Buffer Packing and Unpacking (pack)</title>
    <filename>group__etcpal__pack.html</filename>
    <member kind="function">
      <type>uint16_t</type>
      <name>etcpal_upack_16b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga0880292ba3f725fb8085543b2d423be4</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_16b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga2902019cf1a188b98606319d43ed08a5</anchor>
      <arglist>(uint8_t *buf, uint16_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>etcpal_upack_16l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga8bfbfc050e6a051ca3462c84948a8fcf</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_16l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gac0fc9d22170eb708008eb09f9bb22ced</anchor>
      <arglist>(uint8_t *buf, uint16_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>etcpal_upack_32b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gaac2718bc037c1fb14fc08aa8976b3e0f</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_32b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gab516d6a148228ae6d76de7beb12e60ec</anchor>
      <arglist>(uint8_t *buf, uint32_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>etcpal_upack_32l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gab002038413cf6518988bb965c879e265</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_32l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gacc712186e152a6d7c03cc5f8af5a1cf2</anchor>
      <arglist>(uint8_t *buf, uint32_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint64_t</type>
      <name>etcpal_upack_64b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gac832d56516e61492af5b80013dcb983a</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_64b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga9fd45b2bdbb50c140789f639e74a0a01</anchor>
      <arglist>(uint8_t *buf, uint64_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint64_t</type>
      <name>etcpal_upack_64l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga0d083dae6c2b1c0a04d0512e8fa871d7</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_64l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gaf60d2ae433f57f6cf14f6e54c3aa3a63</anchor>
      <arglist>(uint8_t *buf, uint64_t val)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_pdu</name>
    <title>ACN Protocol Family PDUs (pdu)</title>
    <filename>group__etcpal__pdu.html</filename>
    <subgroup>etcpal_rootlayerpdu</subgroup>
    <class kind="struct">EtcPalPdu</class>
    <class kind="struct">EtcPalPduConstraints</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_LENGTH</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gaa6aa65e575ec892582f10af1a312064e</anchor>
      <arglist>(pdu_buf)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_PACK_NORMAL_LEN</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>ga6c795b26ea3e1c78b545a54e2599c7f9</anchor>
      <arglist>(pdu_buf, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_PACK_EXT_LEN</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gabb06693a6eb3ed13c253c8324f3efbb5</anchor>
      <arglist>(pdu_buf, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_INIT</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gafb7d5c94b80a01d37fa4ec4edcc833c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_INIT_PDU</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>ga0c0d07635b8236c97bb21bbd1eb61c8f</anchor>
      <arglist>(pduptr)</arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalPdu</type>
      <name>EtcPalPdu</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gafd2b4bca589bf922a6458962a1fa1b49</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalPduConstraints</type>
      <name>EtcPalPduConstraints</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gaffe297ca0b89d8b738dad159ccae904c</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_parse_pdu</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>ga9a351fdaedcae30ce8999e4d9fbaf15e</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, const EtcPalPduConstraints *constraints, EtcPalPdu *pdu)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_L_FLAG_SET</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gaadfa588e12322808723c598fef9b2509</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_V_FLAG_SET</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>ga183303aee9717e2c0e662bbfb98dcc1f</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_H_FLAG_SET</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gac6bd98193bb97a1745a87ef1eddec36e</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_D_FLAG_SET</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>ga8395f03212e738c67fee8f6e458617a2</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_SET_L_FLAG</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>ga0aff4c299ef1edd12a62a94791787b05</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_SET_V_FLAG</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gafd5bbef88b8bf08ead91ccf04f44123a</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_SET_H_FLAG</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>ga895f3f23d5fc24feeaf6b9222960a7c0</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_PDU_SET_D_FLAG</name>
      <anchorfile>group__etcpal__pdu.html</anchorfile>
      <anchor>gac7b3e21b8c080aea188a2d577fd31d31</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_rbtree</name>
    <title>Red-Black Trees (rbtree)</title>
    <filename>group__etcpal__rbtree.html</filename>
    <class kind="struct">EtcPalRbNode</class>
    <class kind="struct">EtcPalRbTree</class>
    <class kind="struct">EtcPalRbIter</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_RB_ITER_MAX_HEIGHT</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga347976360fdafc669aab59abc1ff7d73</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalRbIter</type>
      <name>EtcPalRbIter</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gaca406625e86f809e1d5b8e52ca3d3e40</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>etcpal_rbtree_node_cmp_ptr_cb</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga2c7b10e007e2221402426393cbbc3461</anchor>
      <arglist>(const EtcPalRbTree *self, const EtcPalRbNode *a, const EtcPalRbNode *b)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_rbtree_node_dealloc_cb</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga2041c16a4a4e745ccadd180cbb8bc93f</anchor>
      <arglist>(const EtcPalRbTree *self, EtcPalRbNode *node)</arglist>
    </member>
    <member kind="function">
      <type>EtcPalRbNode *</type>
      <name>etcpal_rbnode_init</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gaf901899ba23ab79070d5ed1440440eb2</anchor>
      <arglist>(EtcPalRbNode *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>EtcPalRbTree *</type>
      <name>etcpal_rbtree_init</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga82765df01bbb32545a67ec148536602a</anchor>
      <arglist>(EtcPalRbTree *self, EtcPalRbTreeNodeCmpFunc cmp, EtcPalRbNodeAllocFunc alloc_f, EtcPalRbNodeDeallocFunc dealloc_f)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>etcpal_rbtree_find</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga4e42141273a433aad5e854780e321004</anchor>
      <arglist>(EtcPalRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_rbtree_insert</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gae87df653a152c3942b54a8f8e7c235c6</anchor>
      <arglist>(EtcPalRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_rbtree_remove</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga948b09a65cbe24eefcfb5823641f39f1</anchor>
      <arglist>(EtcPalRbTree *self, void *value)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_rbtree_clear</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga1347a53b2f8505e22707702f490949e8</anchor>
      <arglist>(EtcPalRbTree *self)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_rbtree_size</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gae0aa64287211d3858cef5d0a7cffb033</anchor>
      <arglist>(EtcPalRbTree *self)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_rbtree_insert_node</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga036f8c0aea4ac2050bd2deb904affed6</anchor>
      <arglist>(EtcPalRbTree *self, EtcPalRbNode *node)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_rbtree_remove_with_cb</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gaa2d2391c36849d9a9cd32a0a131f6341</anchor>
      <arglist>(EtcPalRbTree *self, void *value, EtcPalRbTreeNodeFunc node_cb)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_rbtree_clear_with_cb</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga2dfae49efd1e9a9dbd02635bdea8e73d</anchor>
      <arglist>(EtcPalRbTree *self, EtcPalRbTreeNodeFunc node_cb)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>etcpal_rbtree_test</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga3b194e280069ff3fbd105d9799dbea05</anchor>
      <arglist>(EtcPalRbTree *self, EtcPalRbNode *root)</arglist>
    </member>
    <member kind="function">
      <type>EtcPalRbIter *</type>
      <name>etcpal_rbiter_init</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gae7d2f565a40d1ab86873e7bc9c5c5c02</anchor>
      <arglist>(EtcPalRbIter *self)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>etcpal_rbiter_first</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga1cd77fc10e7c983be2c12c5f14bc3721</anchor>
      <arglist>(EtcPalRbIter *self, EtcPalRbTree *tree)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>etcpal_rbiter_last</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga6f1279b41c60d92a808cc9b364f9eac9</anchor>
      <arglist>(EtcPalRbIter *self, EtcPalRbTree *tree)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>etcpal_rbiter_next</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gaa3804b7146d1e8a72e4139a0f4d6507e</anchor>
      <arglist>(EtcPalRbIter *self)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>etcpal_rbiter_prev</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gad093318703094fd4bdb9ecb09443241d</anchor>
      <arglist>(EtcPalRbIter *self)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>EtcPalRbTreeNodeCmpFunc</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gaa92041a856567f1d411d6e2b74233ddc</anchor>
      <arglist>)(const EtcPalRbTree *self, const EtcPalRbNode *node_a, const EtcPalRbNode *node_b)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>EtcPalRbTreeNodeFunc</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga6687d480b9c3f3583303d537190a47de</anchor>
      <arglist>)(const EtcPalRbTree *self, EtcPalRbNode *node)</arglist>
    </member>
    <member kind="typedef">
      <type>EtcPalRbNode *(*</type>
      <name>EtcPalRbNodeAllocFunc</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>ga0f5e94b3a86c36a4a4af25b167cc2990</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>EtcPalRbNodeDeallocFunc</name>
      <anchorfile>group__etcpal__rbtree.html</anchorfile>
      <anchor>gab04d15dd62a53956d6ea4b1e5dcbe930</anchor>
      <arglist>)(EtcPalRbNode *node)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_rootlayerpdu</name>
    <title>ACN Root Layer PDUs (rootlayerpdu)</title>
    <filename>group__etcpal__rootlayerpdu.html</filename>
    <class kind="struct">EtcPalTcpPreamble</class>
    <class kind="struct">EtcPalUdpPreamble</class>
    <class kind="struct">EtcPalRootLayerPdu</class>
    <member kind="define">
      <type>#define</type>
      <name>ACN_TCP_PREAMBLE_SIZE</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>gab879b13847509e3bbd4b12a5e2cda0e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_UDP_PREAMBLE_SIZE</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga86207e41512354a1a6e321d95afe8ab0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_RLP_HEADER_SIZE_NORMAL_LEN</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga29ae7e116cd77cc00366461415bf54a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_RLP_HEADER_SIZE_EXT_LEN</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga9d9a7bd4498a28d5b0de8ed598141c66</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalTcpPreamble</type>
      <name>EtcPalTcpPreamble</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga479d29909ff291c75531e5e45de4af11</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalUdpPreamble</type>
      <name>EtcPalUdpPreamble</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>gabceea25a6773878d73d5c73cff3413db</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalRootLayerPdu</type>
      <name>EtcPalRootLayerPdu</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>gafcee66c76b1dec0bbaf93f9589c0a8dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_parse_tcp_preamble</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga9d42879935ab912ec15d30e9415f5c4b</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, EtcPalTcpPreamble *preamble)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_parse_udp_preamble</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga9c5fe31670c57a02311d3d23f6b96904</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, EtcPalUdpPreamble *preamble)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_parse_root_layer_header</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>gaa454f9d1af3e45a1eefb61c3d4fd9496</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, EtcPalRootLayerPdu *pdu, EtcPalRootLayerPdu *last_pdu)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_parse_root_layer_pdu</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>gac604ba1761a3126ddf9095338ec2d4a4</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, EtcPalRootLayerPdu *pdu, EtcPalPdu *last_pdu)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_pack_tcp_preamble</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga83602488410ff9d31fb781afd5898463</anchor>
      <arglist>(uint8_t *buf, size_t buflen, size_t rlp_block_len)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_pack_udp_preamble</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga205a77f2284495f4b9a16f07dcd43f83</anchor>
      <arglist>(uint8_t *buf, size_t buflen)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_root_layer_buf_size</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga0d9d71dfeb84b94149e146292144615b</anchor>
      <arglist>(const EtcPalRootLayerPdu *pdu_block, size_t num_pdus)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_pack_root_layer_header</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>ga8d5021cdeed9c03dcc28d7f29fbac7d9</anchor>
      <arglist>(uint8_t *buf, size_t buflen, const EtcPalRootLayerPdu *pdu)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_pack_root_layer_block</name>
      <anchorfile>group__etcpal__rootlayerpdu.html</anchorfile>
      <anchor>gabdadb9de2e41045c2010e6085f6ff456</anchor>
      <arglist>(uint8_t *buf, size_t buflen, const EtcPalRootLayerPdu *pdu_block, size_t num_pdus)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_socket</name>
    <title>Network Socket Interface (socket)</title>
    <filename>group__etcpal__socket.html</filename>
    <class kind="struct">EtcPalLinger</class>
    <class kind="struct">EtcPalMreq</class>
    <class kind="struct">EtcPalGroupReq</class>
    <class kind="struct">EtcPalPollEvent</class>
    <class kind="struct">EtcPalAddrinfo</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_POLL_VALID_INPUT_EVENT_MASK</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga05f5bfa7ea20aa326aa97b2424481d99</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SOCKET_INVALID</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga304e26b30c36b6c33319b9602c9fe276</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SOCKET_MAX_POLL_SIZE</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga06b68eaa9d0e50e624659ed1b9c7f05a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>etcpal_poll_events_t</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga625b9f37c09cda6d3f13330094fa97d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalLinger</type>
      <name>EtcPalLinger</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaf5335aecd65e931fa0c9dc0ead350978</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalMreq</type>
      <name>EtcPalMreq</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga3ca1dc73415b552ebca00df10e53a5e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalGroupReq</type>
      <name>EtcPalGroupReq</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga21df9f214a53c2f6b06b15496edfeca8</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalPollEvent</type>
      <name>EtcPalPollEvent</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga7b84da8c4de1182c5b7769a5f5b8034c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalAddrinfo</type>
      <name>EtcPalAddrinfo</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga83e2e233e87a88a7edf037928d203550</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>etcpal_socket_t</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaf08a63353f05072ee3b28fd754985b66</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_accept</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga89be7d2b1e4193d678e36dae7a49b4db</anchor>
      <arglist>(etcpal_socket_t id, EtcPalSockAddr *address, etcpal_socket_t *conn_sock)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_bind</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga86f8e53de9955be4f0ea33f632cc4b2a</anchor>
      <arglist>(etcpal_socket_t id, const EtcPalSockAddr *address)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_close</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaccdfd06b91ca1632a59c68e819270825</anchor>
      <arglist>(etcpal_socket_t id)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_connect</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gac5de9387d7aab2a7aa0e6c4e1d4fee3b</anchor>
      <arglist>(etcpal_socket_t id, const EtcPalSockAddr *address)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_getpeername</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaedab208ccf3be6da95380c5bf01fdf30</anchor>
      <arglist>(etcpal_socket_t id, EtcPalSockAddr *address)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_getsockname</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga3563bac602f434da73673accc8da01aa</anchor>
      <arglist>(etcpal_socket_t id, EtcPalSockAddr *address)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_getsockopt</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga653ed9405ec4cb21d958faab8b1c783e</anchor>
      <arglist>(etcpal_socket_t id, int level, int option_name, void *option_value, size_t *option_len)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_listen</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga39e660822d93588ea8723414567a2415</anchor>
      <arglist>(etcpal_socket_t id, int backlog)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>etcpal_recv</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gace688545de41aecba767639fa0c8f7b6</anchor>
      <arglist>(etcpal_socket_t id, void *buffer, size_t length, int flags)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>etcpal_recvfrom</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga305e1fb78f8ad29e562fff5762d9c3c0</anchor>
      <arglist>(etcpal_socket_t id, void *buffer, size_t length, int flags, EtcPalSockAddr *address)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>etcpal_send</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga52ba67993412de6dc69a582ca07f50d5</anchor>
      <arglist>(etcpal_socket_t id, const void *message, size_t length, int flags)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>etcpal_sendto</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaf13f2f32e223ba9ef28e732cc8906508</anchor>
      <arglist>(etcpal_socket_t id, const void *message, size_t length, int flags, const EtcPalSockAddr *dest_addr)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_setsockopt</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga0cc965b67084fb27214825cb893d2041</anchor>
      <arglist>(etcpal_socket_t id, int level, int option_name, const void *option_value, size_t option_len)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_shutdown</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga3c51a1d50368f372ecd2c44c93f85cac</anchor>
      <arglist>(etcpal_socket_t id, int how)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_socket</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gae0c8d9e036be592eba7db8736c7e1e0f</anchor>
      <arglist>(unsigned int family, unsigned int type, etcpal_socket_t *id)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_setblocking</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga72f16eec0ea5416ac4a553e1e78d7c77</anchor>
      <arglist>(etcpal_socket_t id, bool blocking)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_getblocking</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga7134eafebb4ee73d160cd38d96ddeb9e</anchor>
      <arglist>(etcpal_socket_t id, bool *blocking)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_getaddrinfo</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga5e63f5fa7b1b4310a4bb317aa4873997</anchor>
      <arglist>(const char *hostname, const char *service, const EtcPalAddrinfo *hints, EtcPalAddrinfo *result)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_nextaddr</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga669185d01901034d700f167fa23f3eb1</anchor>
      <arglist>(EtcPalAddrinfo *ai)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_freeaddrinfo</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga8cc1d9b7b5e8e2b88a337fc727b44535</anchor>
      <arglist>(EtcPalAddrinfo *ai)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SOL_SOCKET</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga30a8a29b624b2bef32f68b1fe2aa9e83</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPPROTO_IP</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaba74f8fca47e034d676049bd8eb61488</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPPROTO_ICMPV6</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga66aa3d038609a999ad33370a6c0c3284</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPPROTO_IPV6</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga7db66bcfc3f239baf6a5fef9058192de</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPPROTO_TCP</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaa885355b90b8e92d7f9965b63790810f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPPROTO_UDP</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga19bf0e5a8ce939ea6c4ea034f4839bcc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_BROADCAST</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gab90f6d9eeabe39a57db6b808d24d9bd2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_ERROR</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga20fe86808f28b9a96ee808626e4e3293</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_KEEPALIVE</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga085124fd101ed8c2feb6c224d54d0b39</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_LINGER</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaf47bd53da2c92535aa6edcd8cc820259</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_RCVBUF</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gab07c26f5ae3d4786c824461a65ba0dcc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_SNDBUF</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga7e246a346418dcd9b4318fd673d4b622</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_RCVTIMEO</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga068a8f93c435a18f79b13e21ff30cfd0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_SNDTIMEO</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaf74d842c762e41c9a7e5a5a3404905ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_REUSEADDR</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gac2fc9cd09dce99bb5681df96f65c31b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_REUSEPORT</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga9fb28d8ce3fc93272b41ebe7b5d87989</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SO_TYPE</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gad8abc2b8b8d29d17c94b3493bbf30816</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_TTL</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaa8067f3bea534148c077f2dd720a6d51</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_MULTICAST_IF</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga972636e83a7c9cbc9bb0e5a950c9c241</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_MULTICAST_TTL</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga53c1ba7904c7917177dea985809b12c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_MULTICAST_LOOP</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga0adba5365cbc8344c77ba5a47b43590e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_ADD_MEMBERSHIP</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga19b3047fe621a06b5cedc8043bb823cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_DROP_MEMBERSHIP</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gacbe3547d777f101aaa432e602c4b8e69</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MCAST_JOIN_GROUP</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gacde51cd51d94c43625b96196d7b80f50</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MCAST_LEAVE_GROUP</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga747194309fa23fa0e092e8bd010f1a2c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPV6_V6ONLY</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga6a502de1f396117da63bde901d88e434</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_POLL_IN</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga4e4bec5c7c2860538445ebdba2dd4283</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_POLL_OUT</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga04ebfb989375a6cf612d69f229a74954</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_POLL_CONNECT</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gabbd670e11bf27a8a815b2d7985b9a364</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_POLL_OOB</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga1da970a4c0bd353ec963e4bb70c0f8b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_POLL_ERR</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga837082b33f4ca260e78373ee40bd25ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>EtcPalPollContext</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga906fd2bcb17663f4de3fefd0b335656e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_poll_context_init</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gaf566500c673f2e7941f9a399c61c1b68</anchor>
      <arglist>(EtcPalPollContext *context)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_poll_context_deinit</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga08a886e98949a8c0ba7f162e54cd1a72</anchor>
      <arglist>(EtcPalPollContext *context)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_poll_add_socket</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gab901f9cf32e98e0d99369366f2e1ffa4</anchor>
      <arglist>(EtcPalPollContext *context, etcpal_socket_t socket, etcpal_poll_events_t events, void *user_data)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_poll_modify_socket</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga396b218eda2c3e52b4b917b73e18dab7</anchor>
      <arglist>(EtcPalPollContext *context, etcpal_socket_t socket, etcpal_poll_events_t new_events, void *new_user_data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_poll_remove_socket</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>ga8233886a08aff8ba010dfa7384c76408</anchor>
      <arglist>(EtcPalPollContext *context, etcpal_socket_t socket)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_poll_wait</name>
      <anchorfile>group__etcpal__socket.html</anchorfile>
      <anchor>gae5017e2346be12e91c6450fe9860614b</anchor>
      <arglist>(EtcPalPollContext *context, EtcPalPollEvent *event, int timeout_ms)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_thread</name>
    <title>Threading (thread)</title>
    <filename>group__etcpal__thread.html</filename>
    <class kind="struct">EtcPalThreadParams</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_THREAD_SET_DEFAULT_PARAMS</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>gacb54f3a06ea1b8ae0f45b8daaae37efe</anchor>
      <arglist>(threadparamsptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_THREAD_DEFAULT_PRIORITY</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga3539233977b6e793b9b5a4c071fd6e78</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_THREAD_DEFAULT_STACK</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>gac4147a48a462f0c5b9758334b41243a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_THREAD_DEFAULT_NAME</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga41f1567e754166e758fc1fdda38fc300</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_THREAD_NAME_MAX_LENGTH</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga5a0a37d186ac57231b94c287a58b54a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalThreadParams</type>
      <name>EtcPalThreadParams</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>gabb277d1d3fc2b021ca7513d8af038ed4</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>etcpal_thread_t</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga6ea2bb55f405380e3ea296e3b6164ae8</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_thread_create</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga6622ac7af8e6cc018c28ba15cc824de5</anchor>
      <arglist>(etcpal_thread_t *id, const EtcPalThreadParams *params, void(*thread_fn)(void *), void *thread_arg)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_thread_join</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga8841872731c92356965eac616aedc2ff</anchor>
      <arglist>(etcpal_thread_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_thread_sleep</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga8c76ca9a8ebd49dcb4b376f351fb4517</anchor>
      <arglist>(int sleep_ms)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_timer</name>
    <title>Timers (timer)</title>
    <filename>group__etcpal__timer.html</filename>
    <class kind="struct">EtcPalTimer</class>
    <member kind="typedef">
      <type>struct EtcPalTimer</type>
      <name>EtcPalTimer</name>
      <anchorfile>group__etcpal__timer.html</anchorfile>
      <anchor>gaa8f82d792cb262d907daa7b9586220ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>etcpal_getms</name>
      <anchorfile>group__etcpal__timer.html</anchorfile>
      <anchor>ga9d277421b68caa9f0ad6f23bd775d5d6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_timer_start</name>
      <anchorfile>group__etcpal__timer.html</anchorfile>
      <anchor>ga6e34d82478d4ad222a22df781b5f2f44</anchor>
      <arglist>(EtcPalTimer *timer, uint32_t interval)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_timer_reset</name>
      <anchorfile>group__etcpal__timer.html</anchorfile>
      <anchor>gae3f2a33d174900c2a2a67e2eef842069</anchor>
      <arglist>(EtcPalTimer *timer)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>etcpal_timer_elapsed</name>
      <anchorfile>group__etcpal__timer.html</anchorfile>
      <anchor>ga5d73c9f8144f65719e8b319cdfdc05bb</anchor>
      <arglist>(const EtcPalTimer *timer)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_timer_is_expired</name>
      <anchorfile>group__etcpal__timer.html</anchorfile>
      <anchor>gaccc47c2970648c782918f8ab21d10ae9</anchor>
      <arglist>(const EtcPalTimer *timer)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>etcpal_timer_remaining</name>
      <anchorfile>group__etcpal__timer.html</anchorfile>
      <anchor>ga8f0822e6e65f8e63b3776533ce1ff531</anchor>
      <arglist>(const EtcPalTimer *timer)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_uuid</name>
    <title>UUIDs (uuid)</title>
    <filename>group__etcpal__uuid.html</filename>
    <class kind="struct">EtcPalUuid</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_UUID_BYTES</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>gadfd6e6a51b681c03b6422d1454ddc954</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_UUID_CMP</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga6f2cff87ab29b0b26d0f641b065c998d</anchor>
      <arglist>(uuid1ptr, uuid2ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_UUID_IS_NULL</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga3f7c7f1f596e00536adbefb78899ebd1</anchor>
      <arglist>(uuidptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_UUID_STRING_BYTES</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>gabab14207e273082491f7be2a21275092</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalUuid</type>
      <name>EtcPalUuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga472e79f55f51afda28c54c6967018922</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_uuid_to_string</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga479b00b02ab33cf757e9476ad3286336</anchor>
      <arglist>(const EtcPalUuid *uuid, char *buf)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_string_to_uuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga20f693409bf6b604b0695f811f897cbf</anchor>
      <arglist>(const char *buf, EtcPalUuid *uuid)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_generate_v1_uuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga7a2029b259cc4270fb721b976f21972a</anchor>
      <arglist>(EtcPalUuid *uuid)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_generate_v3_uuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>gadcc004a0b747d06ab79ccc2567a526e1</anchor>
      <arglist>(const char *devstr, const uint8_t *macaddr, uint32_t uuidnum, EtcPalUuid *uuid)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_generate_v4_uuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga1cabb8b6f72752b1a2ae72f776301242</anchor>
      <arglist>(EtcPalUuid *uuid)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_generate_os_preferred_uuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>gabb977b5a218b4b62224c016bf3932f10</anchor>
      <arglist>(EtcPalUuid *uuid)</arglist>
    </member>
    <member kind="variable">
      <type>const EtcPalUuid</type>
      <name>kEtcPalNullUuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>gae518c32bbc7d2266568ee9a703bb4f5e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_error</name>
    <title>Error Handling (error)</title>
    <filename>group__etcpal__cpp__error.html</filename>
    <class kind="class">etcpal::Result</class>
    <class kind="class">etcpal::BadExpectedAccess</class>
    <class kind="class">etcpal::Expected</class>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp</name>
    <title>EtcPal C++ Wrappers</title>
    <filename>group__etcpal__cpp.html</filename>
    <subgroup>etcpal_cpp_error</subgroup>
    <subgroup>etcpal_cpp_inet</subgroup>
    <subgroup>etcpal_cpp_lock</subgroup>
    <subgroup>etcpal_cpp_uuid</subgroup>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_inet</name>
    <title>Internet Addressing (inet)</title>
    <filename>group__etcpal__cpp__inet.html</filename>
    <class kind="class">etcpal::IpAddr</class>
    <class kind="class">etcpal::SockAddr</class>
    <class kind="class">etcpal::MacAddr</class>
    <member kind="enumeration">
      <type></type>
      <name>IpAddrType</name>
      <anchorfile>group__etcpal__cpp__inet.html</anchorfile>
      <anchor>gaf149bcc08653396944a3699b8c5a57c5</anchor>
      <arglist></arglist>
      <enumvalue file="group__etcpal__cpp__inet.html" anchor="ggaf149bcc08653396944a3699b8c5a57c5a4bbb8f967da6d1a610596d7257179c2b">Invalid</enumvalue>
      <enumvalue file="group__etcpal__cpp__inet.html" anchor="ggaf149bcc08653396944a3699b8c5a57c5ab354cab57768c7d4c1106eb4e82c75dc">V4</enumvalue>
      <enumvalue file="group__etcpal__cpp__inet.html" anchor="ggaf149bcc08653396944a3699b8c5a57c5adb93f1c35b844334e2b266951618b4fe">V6</enumvalue>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_lock</name>
    <title>Locks and Synchronization (lock)</title>
    <filename>group__etcpal__cpp__lock.html</filename>
    <class kind="class">etcpal::Mutex</class>
    <class kind="class">etcpal::Signal</class>
    <class kind="class">etcpal::RwLock</class>
    <class kind="class">etcpal::MutexGuard</class>
    <class kind="class">etcpal::ReadGuard</class>
    <class kind="class">etcpal::WriteGuard</class>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_uuid</name>
    <title>UUIDs (uuid)</title>
    <filename>group__etcpal__cpp__uuid.html</filename>
    <class kind="class">etcpal::Uuid</class>
  </compound>
  <compound kind="group">
    <name>etcpal_lock</name>
    <title>Locks and Synchronization (lock)</title>
    <filename>group__etcpal__lock.html</filename>
    <subgroup>etcpal_mutex</subgroup>
    <subgroup>etcpal_signal</subgroup>
    <subgroup>etcpal_rwlock</subgroup>
  </compound>
  <compound kind="group">
    <name>etcpal_mutex</name>
    <title>Mutexes</title>
    <filename>group__etcpal__mutex.html</filename>
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>etcpal_mutex_t</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>ga7360372b14b0feb1ad9f22d2a42734a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_mutex_create</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>ga848f261552bf8c981590eb784ee5b3b8</anchor>
      <arglist>(etcpal_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_mutex_take</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>gac56c0f48e3bfc5e8e0d321a711816f67</anchor>
      <arglist>(etcpal_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_mutex_try_take</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>ga26e7c3d33f3ab2dd5a82de3cbc36d39e</anchor>
      <arglist>(etcpal_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_mutex_give</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>ga5a1ceebcbb5856a98dab50a13f894bcf</anchor>
      <arglist>(etcpal_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_mutex_destroy</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>ga4b71ed71a637a07c0fe5cd55222a209c</anchor>
      <arglist>(etcpal_mutex_t *id)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_signal</name>
    <title>Signals</title>
    <filename>group__etcpal__signal.html</filename>
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>etcpal_signal_t</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>gab450fff060bfcede49357b2e00357669</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_signal_create</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>ga4082ebadfba33b125930d5c5067139f0</anchor>
      <arglist>(etcpal_signal_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_signal_wait</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>ga10e2d2cc15e6a30f38ba65f02c0905d7</anchor>
      <arglist>(etcpal_signal_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_signal_poll</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>gad04fbc96ac93defb7ea545c81a880f12</anchor>
      <arglist>(etcpal_signal_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_signal_post</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>gafa2af190375bdb673563e2a029f0a89b</anchor>
      <arglist>(etcpal_signal_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_signal_destroy</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>ga4ecc28a0f67939859d91c8e9e75701e4</anchor>
      <arglist>(etcpal_signal_t *id)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_rwlock</name>
    <title>Read-Write Locks</title>
    <filename>group__etcpal__rwlock.html</filename>
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>etcpal_rwlock_t</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga5949678b1acff7d9829e257a9bee8d8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_rwlock_create</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga694477ee8932b83784f2d7a70ac856e3</anchor>
      <arglist>(etcpal_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_rwlock_readlock</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga846b8783133884b5fbce918811568825</anchor>
      <arglist>(etcpal_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_rwlock_try_readlock</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga473b7cb0a2bae2deb5477bb699e6cf9b</anchor>
      <arglist>(etcpal_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_rwlock_readunlock</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga3ab0cac4b091255a2694a4a472151160</anchor>
      <arglist>(etcpal_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_rwlock_writelock</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>gac6989af16cb55025d06ee35373739380</anchor>
      <arglist>(etcpal_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_rwlock_try_writelock</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga05a985e996a37e5a4c8aa7f9c3b1d654</anchor>
      <arglist>(etcpal_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_rwlock_writeunlock</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga83fbb82557a70ad86c8a65fd53b0cb8b</anchor>
      <arglist>(etcpal_rwlock_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_rwlock_destroy</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga2d4feb63613ff8dfb1f106f5ed63dee5</anchor>
      <arglist>(etcpal_rwlock_t *id)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_opts</name>
    <title>EtcPal config options</title>
    <filename>group__etcpal__opts.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_INCLUDE_PACK_64</name>
      <anchorfile>group__etcpal__opts.html</anchorfile>
      <anchor>ga5ababb448dd9ec260738821bd16d06cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_EMBOS_USE_MALLOC</name>
      <anchorfile>group__etcpal__opts.html</anchorfile>
      <anchor>gaab67221f3adba4ea2b638e55341f8074</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_EMBOS_MAX_NETINTS</name>
      <anchorfile>group__etcpal__opts.html</anchorfile>
      <anchor>gaf5e80b2cbceca9bbe4e918610021a5ba</anchor>
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
    <title>ETC Platform Abstraction Layer</title>
    <filename>index</filename>
  </compound>
</tagfile>
