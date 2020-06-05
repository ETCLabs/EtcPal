<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>common.h</name>
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2common_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_CONSTEXPR_14</name>
      <anchorfile>group__etcpal__cpp.html</anchorfile>
      <anchor>ga27619ac477b23790da36baecb4389ee1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_CONSTEXPR_14_OR_INLINE</name>
      <anchorfile>group__etcpal__cpp.html</anchorfile>
      <anchor>ga598701b6928c77a8c086183a0a1cb9cc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>error.h</name>
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2error_8h</filename>
    <includes id="cpp_2common_8h" name="common.h" local="yes" imported="no">etcpal/cpp/common.h</includes>
    <class kind="class">etcpal::Error</class>
    <class kind="class">etcpal::BadExpectedAccess</class>
    <class kind="class">etcpal::Expected</class>
  </compound>
  <compound kind="file">
    <name>inet.h</name>
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2inet_8h</filename>
    <includes id="cpp_2common_8h" name="common.h" local="yes" imported="no">etcpal/cpp/common.h</includes>
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
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2lock_8h</filename>
    <class kind="class">etcpal::Mutex</class>
    <class kind="class">etcpal::Signal</class>
    <class kind="class">etcpal::RwLock</class>
    <class kind="class">etcpal::Semaphore</class>
    <class kind="class">etcpal::MutexGuard</class>
    <class kind="class">etcpal::ReadGuard</class>
    <class kind="class">etcpal::WriteGuard</class>
  </compound>
  <compound kind="file">
    <name>log.h</name>
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2log_8h</filename>
    <includes id="cpp_2common_8h" name="common.h" local="yes" imported="no">etcpal/cpp/common.h</includes>
    <includes id="cpp_2inet_8h" name="inet.h" local="yes" imported="no">etcpal/cpp/inet.h</includes>
    <includes id="cpp_2lock_8h" name="lock.h" local="yes" imported="no">etcpal/cpp/lock.h</includes>
    <includes id="cpp_2thread_8h" name="thread.h" local="yes" imported="no">etcpal/cpp/thread.h</includes>
    <class kind="class">etcpal::LogTimestamp</class>
    <class kind="class">etcpal::LogMessageHandler</class>
    <class kind="class">etcpal::Logger</class>
    <member kind="enumeration">
      <type></type>
      <name>LogDispatchPolicy</name>
      <anchorfile>group__etcpal__cpp__log.html</anchorfile>
      <anchor>ga886bd6be55942894f6ddb878d1cfc662</anchor>
      <arglist></arglist>
      <enumvalue file="group__etcpal__cpp__log.html" anchor="gga886bd6be55942894f6ddb878d1cfc662afd1dd0c603be8170f9eae0be9f2f6afb">Direct</enumvalue>
      <enumvalue file="group__etcpal__cpp__log.html" anchor="gga886bd6be55942894f6ddb878d1cfc662a7b2f31b90fe1c2cc33a52233c1925df3">Queued</enumvalue>
    </member>
  </compound>
  <compound kind="file">
    <name>thread.h</name>
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2thread_8h</filename>
    <includes id="cpp_2common_8h" name="common.h" local="yes" imported="no">etcpal/cpp/common.h</includes>
    <includes id="cpp_2error_8h" name="error.h" local="yes" imported="no">etcpal/cpp/error.h</includes>
    <class kind="class">etcpal::Thread</class>
  </compound>
  <compound kind="file">
    <name>timer.h</name>
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2timer_8h</filename>
    <includes id="cpp_2common_8h" name="common.h" local="yes" imported="no">etcpal/cpp/common.h</includes>
    <class kind="class">etcpal::TimePoint</class>
    <class kind="class">etcpal::Timer</class>
  </compound>
  <compound kind="file">
    <name>uuid.h</name>
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/cpp/</path>
    <filename>cpp_2uuid_8h</filename>
    <class kind="class">etcpal::Uuid</class>
    <member kind="enumeration">
      <type></type>
      <name>UuidVersion</name>
      <anchorfile>cpp_2uuid_8h.html</anchorfile>
      <anchor>ac6e0bea8f4b482fd8ab17250626a3cf4</anchor>
      <arglist></arglist>
      <enumvalue file="cpp_2uuid_8h.html" anchor="ac6e0bea8f4b482fd8ab17250626a3cf4ab4daca084ad9eabfc8de231929477ed6">V1</enumvalue>
      <enumvalue file="cpp_2uuid_8h.html" anchor="ac6e0bea8f4b482fd8ab17250626a3cf4aec765f1adc3b4253f2d3b131a4a8618f">V2</enumvalue>
      <enumvalue file="cpp_2uuid_8h.html" anchor="ac6e0bea8f4b482fd8ab17250626a3cf4ad5b50b8cf96bcc8aba90f306f5e6189c">V3</enumvalue>
      <enumvalue file="cpp_2uuid_8h.html" anchor="ac6e0bea8f4b482fd8ab17250626a3cf4ab354cab57768c7d4c1106eb4e82c75dc">V4</enumvalue>
      <enumvalue file="cpp_2uuid_8h.html" anchor="ac6e0bea8f4b482fd8ab17250626a3cf4a8cff5423006469332e8ae5e3a8c8559c">V5</enumvalue>
      <enumvalue file="cpp_2uuid_8h.html" anchor="ac6e0bea8f4b482fd8ab17250626a3cf4a88183b946cc5f0e8c96b2e66e1c74a7e">Unknown</enumvalue>
    </member>
  </compound>
  <compound kind="file">
    <name>version.h</name>
    <path>E:/git/ETCLabs/EtcPal/include/etcpal/</path>
    <filename>version_8h</filename>
  </compound>
  <compound kind="struct">
    <name>AcnPdu</name>
    <filename>struct_acn_pdu.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pvector</name>
      <anchorfile>struct_acn_pdu.html</anchorfile>
      <anchor>a106ad8e138ad9d47e5ed03680c6ac6ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pheader</name>
      <anchorfile>struct_acn_pdu.html</anchorfile>
      <anchor>a89e577801ef19d4cdca129391395ad8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pdata</name>
      <anchorfile>struct_acn_pdu.html</anchorfile>
      <anchor>a2f9eb29aa10e0d64f27f69db1e90bd07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>data_len</name>
      <anchorfile>struct_acn_pdu.html</anchorfile>
      <anchor>ace1c2e33b74df8973a7d9a19c935af80</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pnextpdu</name>
      <anchorfile>struct_acn_pdu.html</anchorfile>
      <anchor>a5af45b626c13232f152d7548f29f9f58</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>AcnPduConstraints</name>
    <filename>struct_acn_pdu_constraints.html</filename>
    <member kind="variable">
      <type>size_t</type>
      <name>vector_size</name>
      <anchorfile>struct_acn_pdu_constraints.html</anchorfile>
      <anchor>a28e18243c88fd99888246e855cc116fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>header_size</name>
      <anchorfile>struct_acn_pdu_constraints.html</anchorfile>
      <anchor>a64f1d0c53909f30b0aac882348cd32f2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>AcnRootLayerPdu</name>
    <filename>struct_acn_root_layer_pdu.html</filename>
    <member kind="variable">
      <type>EtcPalUuid</type>
      <name>sender_cid</name>
      <anchorfile>struct_acn_root_layer_pdu.html</anchorfile>
      <anchor>a341064da4798492e212c14a9aeede58d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>vector</name>
      <anchorfile>struct_acn_root_layer_pdu.html</anchorfile>
      <anchor>a513d751c7097b43b3968ac81f3a5715b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pdata</name>
      <anchorfile>struct_acn_root_layer_pdu.html</anchorfile>
      <anchor>a2f9eb29aa10e0d64f27f69db1e90bd07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>data_len</name>
      <anchorfile>struct_acn_root_layer_pdu.html</anchorfile>
      <anchor>ace1c2e33b74df8973a7d9a19c935af80</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>AcnTcpPreamble</name>
    <filename>struct_acn_tcp_preamble.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>rlp_block</name>
      <anchorfile>struct_acn_tcp_preamble.html</anchorfile>
      <anchor>a6df9d0f27538da177c52996a97e0a1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rlp_block_len</name>
      <anchorfile>struct_acn_tcp_preamble.html</anchorfile>
      <anchor>ab27f79ca664937f91e64f868ad233935</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>AcnUdpPreamble</name>
    <filename>struct_acn_udp_preamble.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>rlp_block</name>
      <anchorfile>struct_acn_udp_preamble.html</anchorfile>
      <anchor>a6df9d0f27538da177c52996a97e0a1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rlp_block_len</name>
      <anchorfile>struct_acn_udp_preamble.html</anchorfile>
      <anchor>ab27f79ca664937f91e64f868ad233935</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::BadExpectedAccess</name>
    <filename>classetcpal_1_1_bad_expected_access.html</filename>
    <member kind="function">
      <type></type>
      <name>BadExpectedAccess</name>
      <anchorfile>classetcpal_1_1_bad_expected_access.html</anchorfile>
      <anchor>a408f3d0470d7f1a3330830eb96db77d9</anchor>
      <arglist>(Error res)</arglist>
    </member>
    <member kind="function">
      <type>Error</type>
      <name>error</name>
      <anchorfile>classetcpal_1_1_bad_expected_access.html</anchorfile>
      <anchor>aa64e8d28a7a913372edd18c2d6a15ab7</anchor>
      <arglist>() const noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::Error</name>
    <filename>classetcpal_1_1_error.html</filename>
    <member kind="function">
      <type>constexpr</type>
      <name>Error</name>
      <anchorfile>classetcpal_1_1_error.html</anchorfile>
      <anchor>acde5c53cc825c9794ffa980ea163a39a</anchor>
      <arglist>(etcpal_error_t code) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Error &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_error.html</anchorfile>
      <anchor>a3c9c46279e5c973e70c51c57cfcaede9</anchor>
      <arglist>(etcpal_error_t code) noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>IsOk</name>
      <anchorfile>classetcpal_1_1_error.html</anchorfile>
      <anchor>a030d244ccd9a40a9f9ac813ea912a639</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr etcpal_error_t</type>
      <name>code</name>
      <anchorfile>classetcpal_1_1_error.html</anchorfile>
      <anchor>a65e890bf700cdd8262c552690a03baad</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>ToString</name>
      <anchorfile>classetcpal_1_1_error.html</anchorfile>
      <anchor>a19c380b03cea21d7ac7325136a131ff0</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>ToCString</name>
      <anchorfile>classetcpal_1_1_error.html</anchorfile>
      <anchor>aa4d860f883bc4c6bc997eb61f218a27e</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>operator bool</name>
      <anchorfile>classetcpal_1_1_error.html</anchorfile>
      <anchor>a7990d8e87d9eab2383f94c88c23a6488</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static constexpr Error</type>
      <name>Ok</name>
      <anchorfile>classetcpal_1_1_error.html</anchorfile>
      <anchor>acbb5c12c9926c6fc2722da18881b8a49</anchor>
      <arglist>() noexcept</arglist>
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
      <type>EtcPalLogCallback</type>
      <name>log_fn</name>
      <anchorfile>struct_etc_pal_log_params.html</anchorfile>
      <anchor>a9a6659e83ffc5e4e7ab3c5ba5009f4e7</anchor>
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
      <type>EtcPalLogTimeFn</type>
      <name>time_fn</name>
      <anchorfile>struct_etc_pal_log_params.html</anchorfile>
      <anchor>aceff50b76400c01d88b158c0a68f32d8</anchor>
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
    <member kind="variable">
      <type>int</type>
      <name>priority</name>
      <anchorfile>struct_etc_pal_log_strings.html</anchorfile>
      <anchor>acec9ce2df15222151ad66fcb1d74eb9f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>EtcPalLogTimestamp</name>
    <filename>struct_etc_pal_log_timestamp.html</filename>
    <member kind="variable">
      <type>unsigned int</type>
      <name>year</name>
      <anchorfile>struct_etc_pal_log_timestamp.html</anchorfile>
      <anchor>a9c83bbc97dcf2390b043382b4dfe3c98</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned int</type>
      <name>month</name>
      <anchorfile>struct_etc_pal_log_timestamp.html</anchorfile>
      <anchor>a8c07e017c7b80f536d05c6134cae89ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned int</type>
      <name>day</name>
      <anchorfile>struct_etc_pal_log_timestamp.html</anchorfile>
      <anchor>a2398ea43f7d25f677616371d3b545f59</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned int</type>
      <name>hour</name>
      <anchorfile>struct_etc_pal_log_timestamp.html</anchorfile>
      <anchor>aa64c18c25aeef9a0d7c92d89fa24a636</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned int</type>
      <name>minute</name>
      <anchorfile>struct_etc_pal_log_timestamp.html</anchorfile>
      <anchor>a7ff63df03ff6549639b712d24a94adef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned int</type>
      <name>second</name>
      <anchorfile>struct_etc_pal_log_timestamp.html</anchorfile>
      <anchor>a7ab95bf164c7490d2855a7efcea6784a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned int</type>
      <name>msec</name>
      <anchorfile>struct_etc_pal_log_timestamp.html</anchorfile>
      <anchor>ab7174bcdc6006eb03894beaa8664521c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>utc_offset</name>
      <anchorfile>struct_etc_pal_log_timestamp.html</anchorfile>
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
      <name>id</name>
      <anchorfile>struct_etc_pal_netint_info.html</anchorfile>
      <anchor>a3548252138686cd633b4730070e2311f</anchor>
      <arglist>[ETCPAL_NETINTINFO_ID_LEN]</arglist>
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
    <name>EtcPalThreadParams</name>
    <filename>struct_etc_pal_thread_params.html</filename>
    <member kind="variable">
      <type>unsigned int</type>
      <name>priority</name>
      <anchorfile>struct_etc_pal_thread_params.html</anchorfile>
      <anchor>a1e440af9e86f7a3c2784c3e2bd687d25</anchor>
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
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a9e2d2226ed3d9558528f40d1e8cdb90a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>af47fca6f0a4334c9331b742efa869cb5</anchor>
      <arglist>(const Expected &amp;other)=default</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>abf3e9a1d1447150ab2f8d6dcdd3e8781</anchor>
      <arglist>(Expected &amp;&amp;other)=default</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a76e78926d166ad8e9d61586963fc078f</anchor>
      <arglist>(const Expected&lt; U &gt; &amp;other, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, const U &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; const U &amp;, T &gt;::value))</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ae4842b36d4f8813847dfc700633b08f0</anchor>
      <arglist>(const Expected&lt; U &gt; &amp;other, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, const U &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;&amp;, T &gt;::value &amp;&amp;std::is_convertible&lt; const U &amp;, T &gt;::value))</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ac20996a0e3daf91215530876a7300f19</anchor>
      <arglist>(Expected&lt; U &gt; &amp;&amp;other, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, U &gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; U, T &gt;::value))</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ab6d89b907f9b5b8773739bb7a1eab687</anchor>
      <arglist>(Expected&lt; U &gt; &amp;&amp;other, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, U &gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; &amp;&amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&gt;::value &amp;&amp;!std::is_constructible&lt; T, Expected&lt; U &gt; const &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; &amp;&amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;, T &gt;::value &amp;&amp;!std::is_convertible&lt; Expected&lt; U &gt; const &amp;&amp;, T &gt;::value &amp;&amp;std::is_convertible&lt; U, T &gt;::value))</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a545cdd9fdc844d352d39aeb345774069</anchor>
      <arglist>(U &amp;&amp;value, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, U &amp;&amp;&gt;::value &amp;&amp;!std::is_convertible&lt; U &amp;&amp;, T &gt;::value)) noexcept(std::is_nothrow_move_constructible&lt; U &gt;::value)</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ac7a8678f6251b90111837a493eef9abe</anchor>
      <arglist>(U &amp;&amp;value, ETCPAL_ENABLE_IF_ARG(std::is_constructible&lt; T, U &amp;&amp;&gt;::value &amp;&amp;std::is_convertible&lt; U &amp;&amp;, T &gt;::value)) noexcept(std::is_nothrow_move_constructible&lt; U &gt;::value)</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>Expected</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ab223fb4af6a34c52a294fb48e37c92c6</anchor>
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
      <type>ETCPAL_CONSTEXPR_14 T &amp;&amp;</type>
      <name>operator*</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>aec2b0f86ca602b33ae1bbe4e72cd571e</anchor>
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
      <type>T &amp;</type>
      <name>value</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a316aefaaaa5e04f8843b07749e04d759</anchor>
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
      <type>ETCPAL_CONSTEXPR_14 T &amp;&amp;</type>
      <name>value</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>abdfb1ce4b51972bd578d02bd8d1daa0c</anchor>
      <arglist>() &amp;&amp;</arglist>
    </member>
    <member kind="function">
      <type>constexpr etcpal_error_t</type>
      <name>error_code</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>ad74bb8607bbb3705c22342bd852e16b8</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr Error</type>
      <name>error</name>
      <anchorfile>classetcpal_1_1_expected.html</anchorfile>
      <anchor>a60850b440a4ff7f769627b46d79043f6</anchor>
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
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>IpAddr</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a523a252e7d40b2a5b8583f5f221adb1d</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>IpAddr</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a28a58c127368fb46de6446c3b17a4efe</anchor>
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
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>IpAddr</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>af9b84482c3f11b17d4824c7214773c75</anchor>
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
      <type>constexpr const EtcPalIpAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a3e6b24d4d6021ab1a11314af504e91cb</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14 EtcPalIpAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a75b91a1739e1ee6a888b8f518288e51d</anchor>
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
      <type>constexpr uint32_t</type>
      <name>v4_data</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>acde4ca75e6e22c1a6899c84d923273db</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr const uint8_t *</type>
      <name>v6_data</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>af7af815dfdf62cfaa17f98743a769028</anchor>
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
      <type>constexpr unsigned long</type>
      <name>scope_id</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a14185dd1b4566147495318504a551034</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>IsValid</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a6d02bb05c3ab899ab6752bbab4e3dbcf</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr IpAddrType</type>
      <name>type</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a6408a893e29a2fcb75d7c6c842db5902</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>IsV4</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a3c808bdaebc8f534c871ac28bf379a7a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>IsV6</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a08a35ea1e7f3f0a6047fbf28810d53e9</anchor>
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
      <type>unsigned int</type>
      <name>MaskLength</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a0613200b3e9958598089ca48824732be</anchor>
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
      <anchor>a431892e2354d1b617e2dd0278b8a9f30</anchor>
      <arglist>(const char *ip_str) noexcept</arglist>
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
    <member kind="function" static="yes">
      <type>static IpAddr</type>
      <name>NetmaskV4</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a0dc66641cafbf7a9f76d26c494bfe189</anchor>
      <arglist>(unsigned int mask_length) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IpAddr</type>
      <name>NetmaskV6</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a69e83da0c99b42fe1be96dede839304d</anchor>
      <arglist>(unsigned int mask_length) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IpAddr</type>
      <name>Netmask</name>
      <anchorfile>classetcpal_1_1_ip_addr.html</anchorfile>
      <anchor>a517378a65c041aa6ee09fc02765a5a97</anchor>
      <arglist>(IpAddrType type, unsigned int mask_length) noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::Logger</name>
    <filename>classetcpal_1_1_logger.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>Startup</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a84e487b1e20733405bd8b870e59f670c</anchor>
      <arglist>(LogMessageHandler &amp;message_handler)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Shutdown</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ac5f038c2b480cf9ef5e19e3eba8dbaf9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>CanLog</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a446621c38cfc4ef6c119c8a26551ccc6</anchor>
      <arglist>(int pri) const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Log</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>aa613ebedf6ebc98cccf35990217e2eab</anchor>
      <arglist>(int pri, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Debug</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ae78231bc988b651151e0f7099a873daa</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Info</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a221110f73de7530eee4f76769cabaf2b</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Notice</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>af0b5bc31187a46a0e5913c493677ba5d</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Warning</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a97d41b6faaf421937e6b82bd1c3067db</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Error</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a4f9ab08aae7973ca510b477041857595</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Critical</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a1c8920f98f8e58c892f8db1b07309c34</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Alert</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a830c42c8d3d96a385510ae78142831e8</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Emergency</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a5a0fec997bc336f87f4897c5a09b5ab1</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>LogDispatchPolicy</type>
      <name>dispatch_policy</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a49d1636c7a4675c4bfc73cf52edacbc2</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>log_mask</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ad0d3bf4ed833ebbaa8529a171d788f20</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>etcpal_log_action_t</type>
      <name>log_action</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a5ec18014ab4cbe6b2cf3ce90e8d6737c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>syslog_facility</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a86ddfb490c635f56281f619b19123eea</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>syslog_hostname</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>aa4d690218c280782b01aff765846c818</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>syslog_app_name</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>acb8556114216b68d3e01d9475f5a589b</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>syslog_procid</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ab671d23235980e85a8c2b952f49aec24</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalLogParams &amp;</type>
      <name>log_params</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a986c589bfc9aa466a31a84a9b8575546</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetDispatchPolicy</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>aeec917862cd49bfea9ea69f0839e78d3</anchor>
      <arglist>(LogDispatchPolicy new_policy) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetLogMask</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>adf3a549fed7e9cf73833af591af43071</anchor>
      <arglist>(int log_mask) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetLogAction</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ad947c8fa600d19b89e362aab5405d07a</anchor>
      <arglist>(etcpal_log_action_t log_action) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogFacility</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a17b3a97c6ab3a6682d84b873edc89b66</anchor>
      <arglist>(int facility) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogHostname</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a96af94f561c632cd17da53244ddd5d8e</anchor>
      <arglist>(const char *hostname) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogHostname</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a59327a03202053f8dd158fb6c2559c26</anchor>
      <arglist>(const std::string &amp;hostname) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogAppName</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a97e218391233bbc428c3e5c2ab5a90bd</anchor>
      <arglist>(const char *app_name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogAppName</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a6a76c7662c48f17ae4bab5cdee6c867a</anchor>
      <arglist>(const std::string &amp;app_name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogProcId</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>adf81d73315c517070a3421a23cbf4c42</anchor>
      <arglist>(const char *proc_id) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogProcId</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ac450da9c26fe790b5e0464b93a2812a8</anchor>
      <arglist>(const std::string &amp;proc_id) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogProcId</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>af8cef80d64e0ac5719b61e726a63a977</anchor>
      <arglist>(int proc_id) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Debug</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ae78231bc988b651151e0f7099a873daa</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Info</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a221110f73de7530eee4f76769cabaf2b</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Notice</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>af0b5bc31187a46a0e5913c493677ba5d</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Warning</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a97d41b6faaf421937e6b82bd1c3067db</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Error</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a4f9ab08aae7973ca510b477041857595</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Critical</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a1c8920f98f8e58c892f8db1b07309c34</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Alert</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a830c42c8d3d96a385510ae78142831e8</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Emergency</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a5a0fec997bc336f87f4897c5a09b5ab1</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>LogDispatchPolicy</type>
      <name>dispatch_policy</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a49d1636c7a4675c4bfc73cf52edacbc2</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>log_mask</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ad0d3bf4ed833ebbaa8529a171d788f20</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>etcpal_log_action_t</type>
      <name>log_action</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a5ec18014ab4cbe6b2cf3ce90e8d6737c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>syslog_facility</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a86ddfb490c635f56281f619b19123eea</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>syslog_hostname</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>aa4d690218c280782b01aff765846c818</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>syslog_app_name</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>acb8556114216b68d3e01d9475f5a589b</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>syslog_procid</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ab671d23235980e85a8c2b952f49aec24</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalLogParams &amp;</type>
      <name>log_params</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a986c589bfc9aa466a31a84a9b8575546</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetDispatchPolicy</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>aeec917862cd49bfea9ea69f0839e78d3</anchor>
      <arglist>(LogDispatchPolicy new_policy) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetLogMask</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>adf3a549fed7e9cf73833af591af43071</anchor>
      <arglist>(int log_mask) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetLogAction</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ad947c8fa600d19b89e362aab5405d07a</anchor>
      <arglist>(etcpal_log_action_t log_action) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogFacility</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a17b3a97c6ab3a6682d84b873edc89b66</anchor>
      <arglist>(int facility) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogHostname</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a96af94f561c632cd17da53244ddd5d8e</anchor>
      <arglist>(const char *hostname) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogHostname</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a59327a03202053f8dd158fb6c2559c26</anchor>
      <arglist>(const std::string &amp;hostname) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogAppName</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a97e218391233bbc428c3e5c2ab5a90bd</anchor>
      <arglist>(const char *app_name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogAppName</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>a6a76c7662c48f17ae4bab5cdee6c867a</anchor>
      <arglist>(const std::string &amp;app_name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogProcId</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>adf81d73315c517070a3421a23cbf4c42</anchor>
      <arglist>(const char *proc_id) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogProcId</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>ac450da9c26fe790b5e0464b93a2812a8</anchor>
      <arglist>(const std::string &amp;proc_id) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Logger &amp;</type>
      <name>SetSyslogProcId</name>
      <anchorfile>classetcpal_1_1_logger.html</anchorfile>
      <anchor>af8cef80d64e0ac5719b61e726a63a977</anchor>
      <arglist>(int proc_id) noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::LogMessageHandler</name>
    <filename>classetcpal_1_1_log_message_handler.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual LogTimestamp</type>
      <name>GetLogTimestamp</name>
      <anchorfile>classetcpal_1_1_log_message_handler.html</anchorfile>
      <anchor>aecfa86ca440c2005c89f7181678eac94</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>HandleLogMessage</name>
      <anchorfile>classetcpal_1_1_log_message_handler.html</anchorfile>
      <anchor>a9d89d39c4e9149f340fe950da112ec6c</anchor>
      <arglist>(const EtcPalLogStrings &amp;strings)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::LogTimestamp</name>
    <filename>classetcpal_1_1_log_timestamp.html</filename>
    <member kind="function">
      <type></type>
      <name>LogTimestamp</name>
      <anchorfile>classetcpal_1_1_log_timestamp.html</anchorfile>
      <anchor>aafc4ef91f7842e34f28549c8b9191376</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>LogTimestamp</name>
      <anchorfile>classetcpal_1_1_log_timestamp.html</anchorfile>
      <anchor>a92d160221d22c19ccf9e027ad770a563</anchor>
      <arglist>(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, unsigned int msec=0, int utc_offset=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsValid</name>
      <anchorfile>classetcpal_1_1_log_timestamp.html</anchorfile>
      <anchor>a472f705194218179e9cf26ffb288da6c</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr const EtcPalLogTimestamp &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_log_timestamp.html</anchorfile>
      <anchor>a45b0c3e59f7392736d9a1f7a91ab6e7e</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14 EtcPalLogTimestamp &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_log_timestamp.html</anchorfile>
      <anchor>a64e9febebf141fb0bc4184c13959ce80</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static LogTimestamp</type>
      <name>Invalid</name>
      <anchorfile>classetcpal_1_1_log_timestamp.html</anchorfile>
      <anchor>ad9fe329da06c0386ffc6fd84b840dcc9</anchor>
      <arglist>()</arglist>
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
      <type>constexpr</type>
      <name>MacAddr</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>a786b90567f5a0f50072a11180bed1e63</anchor>
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
      <type>constexpr const EtcPalMacAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>ab9705cc97de2a4013b231b7bde2ad82b</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14 EtcPalMacAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>a770c9d747f6e9be2eef81c41e926d660</anchor>
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
      <type>constexpr const uint8_t *</type>
      <name>data</name>
      <anchorfile>classetcpal_1_1_mac_addr.html</anchorfile>
      <anchor>a3f4c46c5e6a3a561bad84c6a91ecc8f0</anchor>
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
      <anchor>a32186693c8d1faea349dc007768684b4</anchor>
      <arglist>(const char *mac_str) noexcept</arglist>
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
      <anchor>a8ad93c25cdc665654314bba0797d37ef</anchor>
      <arglist>(int timeout_ms=0)</arglist>
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
      <anchor>ac46abb95ae4ab3f37da632ea31f01535</anchor>
      <arglist>(int timeout_ms=0)</arglist>
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
      <anchor>adfd977b8c35b753a7341a6e64864fa6f</anchor>
      <arglist>(int timeout_ms=0)</arglist>
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
    <name>etcpal::Semaphore</name>
    <filename>classetcpal_1_1_semaphore.html</filename>
    <member kind="function">
      <type></type>
      <name>Semaphore</name>
      <anchorfile>classetcpal_1_1_semaphore.html</anchorfile>
      <anchor>a7afb6e444c74ed583a47dc7bc8bd45e3</anchor>
      <arglist>(unsigned int initial_count=0, unsigned int max_count=kDefaultMaxCount)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Semaphore</name>
      <anchorfile>classetcpal_1_1_semaphore.html</anchorfile>
      <anchor>a5e0394d4ca6109ccb52b3c1e5cf6c9b5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>Wait</name>
      <anchorfile>classetcpal_1_1_semaphore.html</anchorfile>
      <anchor>ae5ccc0d2886026ce37e12604173a131d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>TryWait</name>
      <anchorfile>classetcpal_1_1_semaphore.html</anchorfile>
      <anchor>aa5139da6cf3fc487370524d466cdadd7</anchor>
      <arglist>(int timeout_ms=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>Post</name>
      <anchorfile>classetcpal_1_1_semaphore.html</anchorfile>
      <anchor>a75b89ae344bccade98754cd1fedb13ac</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>PostFromIsr</name>
      <anchorfile>classetcpal_1_1_semaphore.html</anchorfile>
      <anchor>a4e8596ce5216d9d86625212982825670</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>etcpal_sem_t &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_semaphore.html</anchorfile>
      <anchor>a35238d018ce1903b9713675b075f2e38</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static constexpr unsigned int</type>
      <name>kDefaultMaxCount</name>
      <anchorfile>classetcpal_1_1_semaphore.html</anchorfile>
      <anchor>a3486903fdf2712a41832d21fdfd5bc0b</anchor>
      <arglist></arglist>
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
      <name>TryWait</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>aa5139da6cf3fc487370524d466cdadd7</anchor>
      <arglist>(int timeout_ms=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Notify</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>a6b945888ffd146338605d77f0366141b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>NotifyFromIsr</name>
      <anchorfile>classetcpal_1_1_signal.html</anchorfile>
      <anchor>ad8b69b07e8a31350db1cd221db678cb3</anchor>
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
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>acde4b684334acb25ea606acc6a032f09</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a9556543d23d3536e03f7319b50599ddf</anchor>
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
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a469f8a5f9d3f701c84668336a47151c2</anchor>
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
      <type>ETCPAL_CONSTEXPR_14</type>
      <name>SockAddr</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a5522a6d8c6699b7bee4b26cb1cfb7266</anchor>
      <arglist>(IpAddr ip, uint16_t port) noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr const EtcPalSockAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a59860773d22c6c62384a1fe03e774630</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14 EtcPalSockAddr &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>adc9363fc5e38a1d24c5f6c2c41873c8d</anchor>
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
      <type>constexpr IpAddr</type>
      <name>ip</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a3ab600814133b2eb30bfc9d2816d6acd</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr uint16_t</type>
      <name>port</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>abab82fa2b4ca400b38cb9691a2c3fb8a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr uint32_t</type>
      <name>v4_data</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>acde4ca75e6e22c1a6899c84d923273db</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr const uint8_t *</type>
      <name>v6_data</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>af7af815dfdf62cfaa17f98743a769028</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::array&lt; uint8_t, ETCPAL_IPV6_BYTES &gt;</type>
      <name>ToV6Array</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a9d6d96f2d5b5fb9fd6a16bcceb8dea59</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>constexpr unsigned long</type>
      <name>scope_id</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a14185dd1b4566147495318504a551034</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>IsValid</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a6d02bb05c3ab899ab6752bbab4e3dbcf</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr IpAddrType</type>
      <name>type</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a6408a893e29a2fcb75d7c6c842db5902</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>IsV4</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a3c808bdaebc8f534c871ac28bf379a7a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>IsV6</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a08a35ea1e7f3f0a6047fbf28810d53e9</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsLinkLocal</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a211a72caa8aba79efd967fa3114e8c95</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsLoopback</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a4833f2d2e3f0434978f59f5020a7f84b</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsMulticast</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>a2f371a6bc900dd3ddcf0f7f16c5db00a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsWildcard</name>
      <anchorfile>classetcpal_1_1_sock_addr.html</anchorfile>
      <anchor>ac384adc1cbf470fe6672659bdef8358c</anchor>
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
    <name>etcpal::Thread</name>
    <filename>classetcpal_1_1_thread.html</filename>
    <member kind="function">
      <type></type>
      <name>Thread</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>acf0ebde1823ff6854a4add729b40a0c4</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Thread</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a7d7f5dbaad78386dbac48435086454ca</anchor>
      <arglist>(Function &amp;&amp;func, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Thread</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a87a9c69a69ee4b13cb1ec5772f6d32ad</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Thread</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a6fb67adc8253034f42e5a43df0df0662</anchor>
      <arglist>(Thread &amp;&amp;other) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a74ee56b6a23061aaff2a2e34b6035f32</anchor>
      <arglist>(Thread &amp;&amp;other) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Thread</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a856d2ee521709c03d8499d926783117b</anchor>
      <arglist>(const Thread &amp;other)=delete</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>abd79ef28387daa09c6c37909bdff5c69</anchor>
      <arglist>(const Thread &amp;other)=delete</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>joinable</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a2761ee590c5de6d2d9d0045c5d6d9cb7</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>Error</type>
      <name>Start</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a63e85f031a9b180d9e932df21f7a2e64</anchor>
      <arglist>(Function &amp;&amp;func, Args &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>Error</type>
      <name>Join</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a0fe525fdb9bcaccc319c79e1c74ee62e</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>priority</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a22b3fcc6a92ed687e662730c073d9f7d</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>stack_size</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a1e4efbd2bc46e7d5a1b2077ff2038ad8</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>name</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a33256784969e3347f423f72af1727cbd</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>platform_data</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>ac0e98e4ce31becd580ed2ea39cd926b3</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalThreadParams &amp;</type>
      <name>params</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>aa66aa5c50746cd41312f22bebbf0d4e0</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetPriority</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>ac83a10f16662668e1d8a494ddb6e1234</anchor>
      <arglist>(unsigned int priority) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetStackSize</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a3cb4d6197b72897c0ceba341f7beff4b</anchor>
      <arglist>(unsigned int stack_size) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetName</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>ae6cedd23a7b2b24f90059df894846864</anchor>
      <arglist>(const char *name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetName</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>afe34480dc3ef0d8b9f74e389ec1ca647</anchor>
      <arglist>(const std::string &amp;name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetPlatformData</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a23a3a84c2368bdf07cf64e1b49df5364</anchor>
      <arglist>(void *platform_data) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>Sleep</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>abc8bb8ec9948b769e1189506ed635c9d</anchor>
      <arglist>(unsigned int ms) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>Sleep</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a0e7e603ebed62b739065a10cd090213b</anchor>
      <arglist>(const std::chrono::duration&lt; Rep, Period &gt; &amp;sleep_duration) noexcept</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>priority</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a22b3fcc6a92ed687e662730c073d9f7d</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>stack_size</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a1e4efbd2bc46e7d5a1b2077ff2038ad8</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>name</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a33256784969e3347f423f72af1727cbd</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>platform_data</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>ac0e98e4ce31becd580ed2ea39cd926b3</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalThreadParams &amp;</type>
      <name>params</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>aa66aa5c50746cd41312f22bebbf0d4e0</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetPriority</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>ac83a10f16662668e1d8a494ddb6e1234</anchor>
      <arglist>(unsigned int priority) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetStackSize</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a3cb4d6197b72897c0ceba341f7beff4b</anchor>
      <arglist>(unsigned int stack_size) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetName</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>ae6cedd23a7b2b24f90059df894846864</anchor>
      <arglist>(const char *name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetName</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>afe34480dc3ef0d8b9f74e389ec1ca647</anchor>
      <arglist>(const std::string &amp;name) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Thread &amp;</type>
      <name>SetPlatformData</name>
      <anchorfile>classetcpal_1_1_thread.html</anchorfile>
      <anchor>a23a3a84c2368bdf07cf64e1b49df5364</anchor>
      <arglist>(void *platform_data) noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::TimePoint</name>
    <filename>classetcpal_1_1_time_point.html</filename>
    <member kind="function">
      <type></type>
      <name>TimePoint</name>
      <anchorfile>classetcpal_1_1_time_point.html</anchorfile>
      <anchor>a93ecbb20d00c0710c4f157e7742284ec</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>TimePoint</name>
      <anchorfile>classetcpal_1_1_time_point.html</anchorfile>
      <anchor>a9e33e8be45ccfa82649b1bf83c58dbf1</anchor>
      <arglist>(uint32_t ms)</arglist>
    </member>
    <member kind="function">
      <type>constexpr uint32_t</type>
      <name>value</name>
      <anchorfile>classetcpal_1_1_time_point.html</anchorfile>
      <anchor>a5d63d7a2a8500832a76b5f290855db7a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14 TimePoint &amp;</type>
      <name>operator+=</name>
      <anchorfile>classetcpal_1_1_time_point.html</anchorfile>
      <anchor>a6a14c3d43be754d39a7d9c2d99f1474b</anchor>
      <arglist>(uint32_t duration) noexcept</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14 TimePoint &amp;</type>
      <name>operator-=</name>
      <anchorfile>classetcpal_1_1_time_point.html</anchorfile>
      <anchor>a549bf3fd540fc1941bc8a1444f2aa674</anchor>
      <arglist>(uint32_t duration) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static TimePoint</type>
      <name>Now</name>
      <anchorfile>classetcpal_1_1_time_point.html</anchorfile>
      <anchor>ac59e08d917c73620c51f17923428da50</anchor>
      <arglist>() noexcept</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>etcpal::Timer</name>
    <filename>classetcpal_1_1_timer.html</filename>
    <member kind="function">
      <type></type>
      <name>Timer</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a17896e888578cea83217f4bbef07559a</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Timer</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a5928d7d2dc01f531f03c5fcabfeade24</anchor>
      <arglist>(const EtcPalTimer &amp;c_timer) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Timer &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a61c351ff21dcef79e989311e2e2057b2</anchor>
      <arglist>(const EtcPalTimer &amp;c_timer) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Timer</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a3fc5db10dfce0bef1c0eabfd21228c16</anchor>
      <arglist>(uint32_t interval) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Timer</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a300613c1ac94259feced37fe034e1430</anchor>
      <arglist>(const std::chrono::duration&lt; Rep, Period &gt; &amp;interval) noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr const EtcPalTimer &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a78a1cc79e6d7f30343b81d73de6d0c58</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>ETCPAL_CONSTEXPR_14 EtcPalTimer &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>aa2e64a6a8cf23cd9c7c7a6d7510bed8d</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>TimePoint</type>
      <name>GetStartTime</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a2e4c4be67a346b64e3e24b2f6bad80f4</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>GetInterval</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>aa2140520712ad09c27123971fe4bc218</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>GetElapsed</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a8a45a2a5280e8f83c08f6a289b78d482</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>GetRemaining</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>ae12c1dee0d60a4db943b872af5879f76</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IsExpired</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a3a79feefa8df553fd15b9997d0c3a719</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Start</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>ac3e8095b413fff42c74287c5b8fcf17d</anchor>
      <arglist>(uint32_t interval) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Start</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a5128bf0001713abf0599593ac6eba81d</anchor>
      <arglist>(const std::chrono::duration&lt; Rep, Period &gt; &amp;interval) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>Reset</name>
      <anchorfile>classetcpal_1_1_timer.html</anchorfile>
      <anchor>a77ebf418c2f554f19db18aa5adcd0366</anchor>
      <arglist>() noexcept</arglist>
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
      <anchor>aa2f1033714bf728e62eafbc880d502b6</anchor>
      <arglist>(const uint8_t *data) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Uuid</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a65228b364866048f39a365f5aaabcccb</anchor>
      <arglist>(uint32_t time_low, uint16_t time_mid, uint16_t time_hi_and_version, const uint8_t clock_seq_and_node[8])</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Uuid</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a3682e9b132ceba22426927fefdafdeb3</anchor>
      <arglist>(uint32_t time_low, uint16_t time_mid, uint16_t time_hi_and_version, const std::array&lt; uint8_t, 8 &gt; &amp;clock_seq_and_node)</arglist>
    </member>
    <member kind="function">
      <type>constexpr</type>
      <name>Uuid</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a5827f94a1dd0941f5b5842de4034d02b</anchor>
      <arglist>(const EtcPalUuid &amp;c_uuid) noexcept</arglist>
    </member>
    <member kind="function">
      <type>Uuid &amp;</type>
      <name>operator=</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>ada902407e6c698801978a038afb1b4b5</anchor>
      <arglist>(const EtcPalUuid &amp;c_uuid) noexcept</arglist>
    </member>
    <member kind="function">
      <type>constexpr const EtcPalUuid &amp;</type>
      <name>get</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a9df1babf3f2f1a535c4de0c397f92636</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>const uint8_t *</type>
      <name>data</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a21b8e060baa6eeed3e5e45e6d760b54c</anchor>
      <arglist>() const noexcept</arglist>
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
      <anchor>a27909eb1d61b8b261d259be7c70c58dd</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>UuidVersion</type>
      <name>version</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>ad844a4829fe094c9603211c8d13c37bb</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>time_low</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a197e772363eb4fb42ca981f6690cc6eb</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>time_mid</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a5a3534e539c75eb5add173f7f660d45a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>time_hi_and_version</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a0fd84d72db93c35ce9d86a1fff1b1067</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::array&lt; uint8_t, 8 &gt;</type>
      <name>clock_seq_and_node</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>ad1c7dfeea57e4e696bca56b1cb172292</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>FromString</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>ab017791f5f66829527591d8bd6bd840f</anchor>
      <arglist>(const char *uuid_str) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>FromString</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a824ab92916f13aacf1899f5019093470</anchor>
      <arglist>(const std::string &amp;uuid_str) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V1</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a05ce6a071ffc17d19fbc7818614af8d0</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V3</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>acf48b871ad6aa14afc1a80cf688ae6f8</anchor>
      <arglist>(const Uuid &amp;ns, const void *name, size_t name_len) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V3</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a654bace3b7b14715143dd4a955f477c5</anchor>
      <arglist>(const Uuid &amp;ns, const char *name) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V3</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a637aba4ac439b6c6845f6ff8e3411186</anchor>
      <arglist>(const Uuid &amp;ns, const std::string &amp;name) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V4</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a3bcee74617519b972666b57ff986bf43</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V5</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>ae4e59b0f73518f92ed7c3c6d1b65bd3c</anchor>
      <arglist>(const Uuid &amp;ns, const void *name, size_t name_len) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V5</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a729621c06a6d1bd895a9c82b30141c9a</anchor>
      <arglist>(const Uuid &amp;ns, const char *name) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>V5</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>acaa3b4ab280e85d5c18c2749018c6153</anchor>
      <arglist>(const Uuid &amp;ns, const std::string &amp;name) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>OsPreferred</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>ae1ccf480526751a937afb32a2db7346a</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>Device</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a30a2f58b68d4d0e7d2ae6a63213a388a</anchor>
      <arglist>(const std::string &amp;device_str, const uint8_t *mac_addr, uint32_t uuid_num) noexcept</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Uuid</type>
      <name>Device</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a2d8ba319ae1c2e12b6ed12742562d038</anchor>
      <arglist>(const std::string &amp;device_str, const std::array&lt; uint8_t, 6 &gt; &amp;mac_addr, uint32_t uuid_num) noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>time_low</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a197e772363eb4fb42ca981f6690cc6eb</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>time_mid</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a5a3534e539c75eb5add173f7f660d45a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>time_hi_and_version</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>a0fd84d72db93c35ce9d86a1fff1b1067</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>std::array&lt; uint8_t, 8 &gt;</type>
      <name>clock_seq_and_node</name>
      <anchorfile>classetcpal_1_1_uuid.html</anchorfile>
      <anchor>ad1c7dfeea57e4e696bca56b1cb172292</anchor>
      <arglist>() const noexcept</arglist>
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
    <name>etcpal_acn_pdu</name>
    <title>acn_pdu (ACN Protocol Family PDUs)</title>
    <filename>group__etcpal__acn__pdu.html</filename>
    <class kind="struct">AcnPdu</class>
    <class kind="struct">AcnPduConstraints</class>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_LENGTH</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>gae56bb8381a08d26f7a124eb82a65ce70</anchor>
      <arglist>(pdu_buf)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_PACK_NORMAL_LEN</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>ga0c52657116f8cbc6376e4185aa10ed4e</anchor>
      <arglist>(pdu_buf, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_PACK_EXT_LEN</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>gaf63c612f16b76b5d1f56d569aa06de39</anchor>
      <arglist>(pdu_buf, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_INIT</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>ga408e2eafbb588ee9138013a62c412e4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_INIT_PDU</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>ga9c8ce768d1162956568cf55f2c42d6bb</anchor>
      <arglist>(pduptr)</arglist>
    </member>
    <member kind="typedef">
      <type>struct AcnPdu</type>
      <name>AcnPdu</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>ga3628638e45db8db2868d39815491d690</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct AcnPduConstraints</type>
      <name>AcnPduConstraints</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>ga30e387a60bcb97095e371f5fca5b5962</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>acn_parse_pdu</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>gab277ac11752967a19ded43f02e8bf0d4</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, const AcnPduConstraints *constraints, AcnPdu *pdu)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_L_FLAG_SET</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>gaff1b6f9a3c23b53b3dc32b8fba814e84</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_V_FLAG_SET</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>gabee5ccd32aebcd51a251a606c1059e6a</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_H_FLAG_SET</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>ga4e16a5ba393080866e3bf05184bdd8cc</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_D_FLAG_SET</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>gabfbc18cb41624f62faf120c756e15a93</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_SET_L_FLAG</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>gadb9812c9527ce0e9990ef2f4a7ba7384</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_SET_V_FLAG</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>gaf4b41a1aef5cd04be19b8dcb747445f5</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_SET_H_FLAG</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>ga242de4c8a855bf849a2571f4f959f39c</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_PDU_SET_D_FLAG</name>
      <anchorfile>group__etcpal__acn__pdu.html</anchorfile>
      <anchor>ga81c687ecab08faeb07309f0a105427fd</anchor>
      <arglist>(flags_byte)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_acn_rlp</name>
    <title>acn_rlp (ACN Root Layer Protocol)</title>
    <filename>group__etcpal__acn__rlp.html</filename>
    <class kind="struct">AcnTcpPreamble</class>
    <class kind="struct">AcnUdpPreamble</class>
    <class kind="struct">AcnRootLayerPdu</class>
    <member kind="define">
      <type>#define</type>
      <name>ACN_TCP_PREAMBLE_SIZE</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>gab879b13847509e3bbd4b12a5e2cda0e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_UDP_PREAMBLE_SIZE</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga86207e41512354a1a6e321d95afe8ab0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_RLP_HEADER_SIZE_NORMAL_LEN</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga29ae7e116cd77cc00366461415bf54a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ACN_RLP_HEADER_SIZE_EXT_LEN</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga9d9a7bd4498a28d5b0de8ed598141c66</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct AcnTcpPreamble</type>
      <name>AcnTcpPreamble</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga548213c8a55f62ef6d1aaebebf5d9d92</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct AcnUdpPreamble</type>
      <name>AcnUdpPreamble</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>gac581ac5bbc25e6c544f10a718bb6a6d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct AcnRootLayerPdu</type>
      <name>AcnRootLayerPdu</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga1f5c4d94231529fba7aa1c852cde9665</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>acn_parse_tcp_preamble</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>gab2edaace9af15cb4dd54bfb8ad36fe88</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, AcnTcpPreamble *preamble)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>acn_parse_udp_preamble</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga14db47bbc1098cb4dfa4826f7a121d92</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, AcnUdpPreamble *preamble)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>acn_parse_root_layer_header</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga0174e7b359b079e56c0ac114f43d4161</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, AcnRootLayerPdu *pdu, AcnRootLayerPdu *last_pdu)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>acn_parse_root_layer_pdu</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>gabf21b75d1b5bfe26178b328c3cd2fa8d</anchor>
      <arglist>(const uint8_t *buf, size_t buflen, AcnRootLayerPdu *pdu, AcnPdu *last_pdu)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>acn_pack_tcp_preamble</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga615060ff8e3ed0811418b60498872177</anchor>
      <arglist>(uint8_t *buf, size_t buflen, size_t rlp_block_len)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>acn_pack_udp_preamble</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>gad1ebc44ac8675cea86bd5fa4a2f5a530</anchor>
      <arglist>(uint8_t *buf, size_t buflen)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>acn_root_layer_buf_size</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga76d83773df9894ecbaf8b505647c32e5</anchor>
      <arglist>(const AcnRootLayerPdu *pdu_block, size_t num_pdus)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>acn_pack_root_layer_header</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>gaf4281e58f075ee0e14f2e6fa50f059b9</anchor>
      <arglist>(uint8_t *buf, size_t buflen, const AcnRootLayerPdu *pdu)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>acn_pack_root_layer_block</name>
      <anchorfile>group__etcpal__acn__rlp.html</anchorfile>
      <anchor>ga3b7afd5a73e12e1acd650455c7e22997</anchor>
      <arglist>(uint8_t *buf, size_t buflen, const AcnRootLayerPdu *pdu_block, size_t num_pdus)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal</name>
    <title>etcpal (Core Modules)</title>
    <filename>group__etcpal.html</filename>
    <subgroup>etcpal_acn_pdu</subgroup>
    <subgroup>etcpal_acn_rlp</subgroup>
    <subgroup>etcpal_error</subgroup>
    <subgroup>etcpal_inet</subgroup>
    <subgroup>etcpal_log</subgroup>
    <subgroup>etcpal_mempool</subgroup>
    <subgroup>etcpal_netint</subgroup>
    <subgroup>etcpal_pack</subgroup>
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
    <title>error (Error Handling)</title>
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
    <title>inet (Internet Addressing)</title>
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
      <name>ETCPAL_NETINTINFO_ID_LEN</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga7d30e1fd482aa981c7ee4a5082d6bc2d</anchor>
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
      <name>ETCPAL_IP_STRING_BYTES</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gacd7ec0d81e171166483c7629669bf05d</anchor>
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
      <name>etcpal_ip_to_string</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaf98da0ae21f7960b5e046c55b8d79c4a</anchor>
      <arglist>(const EtcPalIpAddr *src, char *dest)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_string_to_ip</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga92ba01cb2c81a8a97b9555ecf0ab2fe5</anchor>
      <arglist>(etcpal_iptype_t type, const char *src, EtcPalIpAddr *dest)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_mac_to_string</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaf1e26c55e9ece4a8a17bd9953eca0886</anchor>
      <arglist>(const EtcPalMacAddr *src, char *dest)</arglist>
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
      <name>ETCPAL_IP_INVALID_INIT_VALUES</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gaeffa39a3f4885c198b5ed7f49463d0b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IP_INVALID_INIT</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gad8ef4406c72bd87baad2ce085fb29daf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPV4_INIT_VALUES</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga19607012ab4fcba516b8f59d8d1df840</anchor>
      <arglist>(v4_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPV4_INIT</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga5a003e68fd0875858bf7dcdfad05db3f</anchor>
      <arglist>(addr_val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPV6_INIT_VALUES</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>gab26d4b7b0dd23818f02a54be33fa2bd2</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_IPV6_INIT</name>
      <anchorfile>group__etcpal__inet.html</anchorfile>
      <anchor>ga66c5a1296b2777b56a9caa013a6a67aa</anchor>
      <arglist>(...)</arglist>
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
    <name>etcpal_log</name>
    <title>log (Logging)</title>
    <filename>group__etcpal__log.html</filename>
    <class kind="struct">EtcPalLogTimestamp</class>
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
      <name>ETCPAL_RAW_LOG_MSG_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga23f61f04ce3dd5ee8ceaf88f99121999</anchor>
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
      <name>ETCPAL_LOG_STR_MIN_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga5f7e222e2b3359ca02363b27f38e97b2</anchor>
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
      <name>ETCPAL_LOG_STR_MAX_LEN</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaec05f1fdff3420181c0fe5ae92648e68</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct EtcPalLogTimestamp</type>
      <name>EtcPalLogTimestamp</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga6ce3e6fd83a776f1696163386ed4fde0</anchor>
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
      <name>EtcPalLogCallback</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaf8d8eef393a51fca0f1ddbfd7cad9c82</anchor>
      <arglist>)(void *context, const EtcPalLogStrings *strings)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>EtcPalLogTimeFn</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gae62cd1cd05e3e42c5df41cab8df8e9e3</anchor>
      <arglist>)(void *context, EtcPalLogTimestamp *timestamp)</arglist>
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
      <name>kEtcPalLogCreateHumanReadable</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gga10855f3c1f3816d0bd5e4687108ea1c5aa0ece7bdc03dd5b29a3be01c212f6b48</anchor>
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
      <name>etcpal_create_log_str</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gacf54b7ce272d89f2a35a83c4cd3bda4f</anchor>
      <arglist>(char *buf, size_t buflen, const EtcPalLogTimestamp *timestamp, int pri, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_vcreate_log_str</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gadec7b676650d22a5315d97f3aeee1dd0</anchor>
      <arglist>(char *buf, size_t buflen, const EtcPalLogTimestamp *timestamp, int pri, const char *format, va_list args)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_create_syslog_str</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>gaac4f27cd13defcb0fbd39516b5d5844d</anchor>
      <arglist>(char *buf, size_t buflen, const EtcPalLogTimestamp *timestamp, const EtcPalSyslogParams *syslog_params, int pri, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_vcreate_syslog_str</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga943bb13f1d71efce45b30b466e757138</anchor>
      <arglist>(char *buf, size_t buflen, const EtcPalLogTimestamp *timestamp, const EtcPalSyslogParams *syslog_params, int pri, const char *format, va_list args)</arglist>
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
      <name>etcpal_validate_log_timestamp</name>
      <anchorfile>group__etcpal__log.html</anchorfile>
      <anchor>ga8bdf4e7812a1fe1d3b2c5da3ffc4f028</anchor>
      <arglist>(const EtcPalLogTimestamp *timestamp)</arglist>
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
    <title>mempool (Memory Pools)</title>
    <filename>group__etcpal__mempool.html</filename>
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
  </compound>
  <compound kind="group">
    <name>etcpal_netint</name>
    <title>netint (Network Interfaces)</title>
    <filename>group__etcpal__netint.html</filename>
    <member kind="function">
      <type>size_t</type>
      <name>etcpal_netint_get_num_interfaces</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>ga4b952bf9d2c5c21e62c691c722730a39</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const EtcPalNetintInfo *</type>
      <name>etcpal_netint_get_interfaces</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>ga32d2c099254727efd5a2d8208a7a88e4</anchor>
      <arglist>(void)</arglist>
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
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_netint_refresh_interfaces</name>
      <anchorfile>group__etcpal__netint.html</anchorfile>
      <anchor>ga4708e2c4209e3a74e6c22d2c02c52f62</anchor>
      <arglist>(bool *list_changed)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_pack</name>
    <title>pack (Buffer Packing and Unpacking)</title>
    <filename>group__etcpal__pack.html</filename>
    <member kind="function">
      <type>int16_t</type>
      <name>etcpal_unpack_i16b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga03cb215e85989e72c68d5375c3ba497a</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_i16b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga643eda014d76e31b7b814740477129b6</anchor>
      <arglist>(uint8_t *buf, int16_t val)</arglist>
    </member>
    <member kind="function">
      <type>int16_t</type>
      <name>etcpal_unpack_i16l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga9665c39fc1edb5c993a65d1bc0227eca</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_i16l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gab6baf283ded8edfee3eea443c8494b00</anchor>
      <arglist>(uint8_t *buf, int16_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>etcpal_unpack_u16b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gad3f7ba35fa369d1749bff71ef704fe3d</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_u16b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gad3d316c55c7673d619bbb92a27eaab5c</anchor>
      <arglist>(uint8_t *buf, uint16_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>etcpal_unpack_u16l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gaad71f04b710d89d536ed3d01c75b3d94</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_u16l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga79214e42a36a2cc967fc417e82aa3ecf</anchor>
      <arglist>(uint8_t *buf, uint16_t val)</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>etcpal_unpack_i32b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga36f5a90e3fe43a44a33a4d8ea7550f6f</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_i32b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga6195295fa17c918934dd7ad960356f53</anchor>
      <arglist>(uint8_t *buf, int32_t val)</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>etcpal_unpack_i32l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gadcd258a8510cb2e25f916c03e2f3cf61</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_i32l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga3d81cfd5ea0db9f904260c9269ae90e6</anchor>
      <arglist>(uint8_t *buf, int32_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>etcpal_unpack_u32b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga3a0676b70d6f218eb57a2b43a5556fa9</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_u32b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga88e2c7307bc91f9936fcc13c63c02631</anchor>
      <arglist>(uint8_t *buf, uint32_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>etcpal_unpack_u32l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga08dcb3a3db0e2503288fc0510dacb607</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_u32l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga19ac23cec592f0bd81baa2fda9ae2336</anchor>
      <arglist>(uint8_t *buf, uint32_t val)</arglist>
    </member>
    <member kind="function">
      <type>int64_t</type>
      <name>etcpal_unpack_i64b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga561ee21a6f18638da0ac63c6c564db3b</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_i64b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gaad4afcee267b57a85444bdceb3115d46</anchor>
      <arglist>(uint8_t *buf, int64_t val)</arglist>
    </member>
    <member kind="function">
      <type>int64_t</type>
      <name>etcpal_unpack_i64l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gabf19d28117a02f5d9365f0acaae51b8b</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_i64l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga1a7f6367dcee8974cb5e2ae1dfcced3d</anchor>
      <arglist>(uint8_t *buf, int64_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint64_t</type>
      <name>etcpal_unpack_u64b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga60cf532cf79359bdee595b049627a1d1</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_u64b</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>ga544c2b2ae7ba6f46758298719de9d2e8</anchor>
      <arglist>(uint8_t *buf, uint64_t val)</arglist>
    </member>
    <member kind="function">
      <type>uint64_t</type>
      <name>etcpal_unpack_u64l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gafa3382e49751478fe5348e23b7c82dc1</anchor>
      <arglist>(const uint8_t *buf)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_pack_u64l</name>
      <anchorfile>group__etcpal__pack.html</anchorfile>
      <anchor>gaa220a31557370465e1be3f7bdc53d0fe</anchor>
      <arglist>(uint8_t *buf, uint64_t val)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_rbtree</name>
    <title>rbtree (Red-Black Trees)</title>
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
      <anchor>gabda4a5620ad7cddcc09ef71d4d8222c7</anchor>
      <arglist>(const EtcPalRbTree *self, const void *a, const void *b)</arglist>
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
      <anchor>ga4d3d93cb20130fc9d34fc13d1ea92316</anchor>
      <arglist>(EtcPalRbTree *self, const void *value)</arglist>
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
      <anchor>ga46d80469b4a700447b2721589cbec3d9</anchor>
      <arglist>(EtcPalRbTree *self, const void *value)</arglist>
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
      <anchor>gae9045e863548f8051541f3f054bbf4be</anchor>
      <arglist>(EtcPalRbTree *self, const void *value, EtcPalRbTreeNodeFunc node_cb)</arglist>
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
      <anchor>ga0f5fd5d6b8505a1e408a8f22fb3ede2b</anchor>
      <arglist>)(const EtcPalRbTree *self, const void *value_a, const void *value_b)</arglist>
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
      <anchor>ga7e715765639ff5a6b66065858541b083</anchor>
      <arglist>)(void)</arglist>
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
    <name>etcpal_socket</name>
    <title>socket (Network Socket Interface)</title>
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
    <title>thread (Threading)</title>
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
      <name>ETCPAL_THREAD_PARAMS_INIT_VALUES</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>gacbaaaa3fb12075714725a11874b6b4ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_THREAD_PARAMS_INIT</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga7f21ced82782c355a30a8ad011aa5b91</anchor>
      <arglist></arglist>
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
      <type>etcpal_error_t</type>
      <name>etcpal_thread_create</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga16f3116e3a533530d44c0d47b263f8e9</anchor>
      <arglist>(etcpal_thread_t *id, const EtcPalThreadParams *params, void(*thread_fn)(void *), void *thread_arg)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_thread_join</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>ga711be33696701513f19239cf22dc3792</anchor>
      <arglist>(etcpal_thread_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_thread_sleep</name>
      <anchorfile>group__etcpal__thread.html</anchorfile>
      <anchor>gaf5d7c5aec312d172e617fd653ade3b08</anchor>
      <arglist>(unsigned int sleep_ms)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_timer</name>
    <title>timer (Timers)</title>
    <filename>group__etcpal__timer.html</filename>
    <class kind="struct">EtcPalTimer</class>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_TIME_ELAPSED_SINCE</name>
      <anchorfile>group__etcpal__timer.html</anchorfile>
      <anchor>ga90572188aaa8e169a70ad52ce251ba9c</anchor>
      <arglist>(start_time)</arglist>
    </member>
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
      <anchor>gad9f192bf100bebdab2d0169c44af066a</anchor>
      <arglist>(void)</arglist>
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
    <title>uuid (UUIDs)</title>
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
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_UUID_DEV_STR_MAX_LEN</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga191ac9542f1d4eaa95488b3740bb57bf</anchor>
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
      <anchor>ga8da6cbed139b61853a98f41120e73b29</anchor>
      <arglist>(const char *str, EtcPalUuid *uuid)</arglist>
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
      <anchor>gacede094cfd672660efa757fa2625e083</anchor>
      <arglist>(const EtcPalUuid *ns, const void *name, size_t name_len, EtcPalUuid *uuid)</arglist>
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
      <name>etcpal_generate_v5_uuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>gac74a0ebbade4f8c2b90a778529176c9b</anchor>
      <arglist>(const EtcPalUuid *ns, const void *name, size_t name_len, EtcPalUuid *uuid)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_generate_os_preferred_uuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>gabb977b5a218b4b62224c016bf3932f10</anchor>
      <arglist>(EtcPalUuid *uuid)</arglist>
    </member>
    <member kind="function">
      <type>etcpal_error_t</type>
      <name>etcpal_generate_device_uuid</name>
      <anchorfile>group__etcpal__uuid.html</anchorfile>
      <anchor>ga1a5104de6a3c54113edf234e0840dcda</anchor>
      <arglist>(const char *dev_str, const uint8_t *mac_addr, uint32_t uuid_num, EtcPalUuid *uuid)</arglist>
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
    <name>etcpal_cpp</name>
    <title>etcpal/cpp (C++ Wrappers)</title>
    <filename>group__etcpal__cpp.html</filename>
    <subgroup>etcpal_cpp_error</subgroup>
    <subgroup>etcpal_cpp_inet</subgroup>
    <subgroup>etcpal_cpp_lock</subgroup>
    <subgroup>etcpal_cpp_log</subgroup>
    <subgroup>etcpal_cpp_thread</subgroup>
    <subgroup>etcpal_cpp_timer</subgroup>
    <subgroup>etcpal_cpp_uuid</subgroup>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_CONSTEXPR_14</name>
      <anchorfile>group__etcpal__cpp.html</anchorfile>
      <anchor>ga27619ac477b23790da36baecb4389ee1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_CONSTEXPR_14_OR_INLINE</name>
      <anchorfile>group__etcpal__cpp.html</anchorfile>
      <anchor>ga598701b6928c77a8c086183a0a1cb9cc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_error</name>
    <title>error (Error Handling)</title>
    <filename>group__etcpal__cpp__error.html</filename>
    <class kind="class">etcpal::Error</class>
    <class kind="class">etcpal::BadExpectedAccess</class>
    <class kind="class">etcpal::Expected</class>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_inet</name>
    <title>inet (Internet Addressing)</title>
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
    <title>lock (Locks and Synchronization)</title>
    <filename>group__etcpal__cpp__lock.html</filename>
    <class kind="class">etcpal::Mutex</class>
    <class kind="class">etcpal::Signal</class>
    <class kind="class">etcpal::RwLock</class>
    <class kind="class">etcpal::Semaphore</class>
    <class kind="class">etcpal::MutexGuard</class>
    <class kind="class">etcpal::ReadGuard</class>
    <class kind="class">etcpal::WriteGuard</class>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_log</name>
    <title>log (Logging)</title>
    <filename>group__etcpal__cpp__log.html</filename>
    <class kind="class">etcpal::LogTimestamp</class>
    <class kind="class">etcpal::LogMessageHandler</class>
    <class kind="class">etcpal::Logger</class>
    <member kind="enumeration">
      <type></type>
      <name>LogDispatchPolicy</name>
      <anchorfile>group__etcpal__cpp__log.html</anchorfile>
      <anchor>ga886bd6be55942894f6ddb878d1cfc662</anchor>
      <arglist></arglist>
      <enumvalue file="group__etcpal__cpp__log.html" anchor="gga886bd6be55942894f6ddb878d1cfc662afd1dd0c603be8170f9eae0be9f2f6afb">Direct</enumvalue>
      <enumvalue file="group__etcpal__cpp__log.html" anchor="gga886bd6be55942894f6ddb878d1cfc662a7b2f31b90fe1c2cc33a52233c1925df3">Queued</enumvalue>
    </member>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_thread</name>
    <title>thread (Threading)</title>
    <filename>group__etcpal__cpp__thread.html</filename>
    <class kind="class">etcpal::Thread</class>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_timer</name>
    <title>timer (Timers)</title>
    <filename>group__etcpal__cpp__timer.html</filename>
    <class kind="class">etcpal::TimePoint</class>
    <class kind="class">etcpal::Timer</class>
  </compound>
  <compound kind="group">
    <name>etcpal_cpp_uuid</name>
    <title>uuid (UUIDs)</title>
    <filename>group__etcpal__cpp__uuid.html</filename>
    <class kind="class">etcpal::Uuid</class>
  </compound>
  <compound kind="group">
    <name>etcpal_lock</name>
    <title>lock (Locks and Synchronization)</title>
    <filename>group__etcpal__lock.html</filename>
    <subgroup>etcpal_mutex</subgroup>
    <subgroup>etcpal_signal</subgroup>
    <subgroup>etcpal_rwlock</subgroup>
    <subgroup>etcpal_sem</subgroup>
  </compound>
  <compound kind="group">
    <name>etcpal_mutex</name>
    <title>Mutexes</title>
    <filename>group__etcpal__mutex.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_MUTEX_HAS_TIMED_LOCK</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>ga8c452b6fbbfffb5071cbb62c2011cd68</anchor>
      <arglist></arglist>
    </member>
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
      <name>etcpal_mutex_lock</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>gab53dcdc649c23fb797228f414bb67e34</anchor>
      <arglist>(etcpal_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_mutex_try_lock</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>gac3095a52d1afa4b44ffa066a9dde05a5</anchor>
      <arglist>(etcpal_mutex_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_mutex_timed_lock</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>ga94809c21d2ddd2ab62bbb7c7d3f7f576</anchor>
      <arglist>(etcpal_mutex_t *id, int timeout_ms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_mutex_unlock</name>
      <anchorfile>group__etcpal__mutex.html</anchorfile>
      <anchor>ga8468c318f6d0737460888bfd6ddd3fe5</anchor>
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
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SIGNAL_HAS_TIMED_WAIT</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>gae27ec551b3bcdeb4c72e6ecea9ba3ca8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SIGNAL_HAS_POST_FROM_ISR</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>ga29bf79c6470cde2c5bdc45ab90f4d826</anchor>
      <arglist></arglist>
    </member>
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
      <name>etcpal_signal_try_wait</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>gaae6c46b47ab125aeac18bf0a27bc6f9d</anchor>
      <arglist>(etcpal_signal_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_signal_timed_wait</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>ga770f7b128af027172c866a765f50f4fb</anchor>
      <arglist>(etcpal_signal_t *id, int timeout_ms)</arglist>
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
      <name>etcpal_signal_post_from_isr</name>
      <anchorfile>group__etcpal__signal.html</anchorfile>
      <anchor>gad88439176943dd91e5a98f0f508c55a1</anchor>
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
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_RWLOCK_HAS_TIMED_LOCK</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga076630adff898bb71a4aaca77a73aefc</anchor>
      <arglist></arglist>
    </member>
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
      <type>bool</type>
      <name>etcpal_rwlock_timed_readlock</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>gafa8f15715dac3b6f54bd7dcafe62da10</anchor>
      <arglist>(etcpal_rwlock_t *id, int timeout_ms)</arglist>
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
      <type>bool</type>
      <name>etcpal_rwlock_timed_writelock</name>
      <anchorfile>group__etcpal__rwlock.html</anchorfile>
      <anchor>ga0f848288a067dbfd5348c089c1dbb76b</anchor>
      <arglist>(etcpal_rwlock_t *id, int timeout_ms)</arglist>
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
    <name>etcpal_sem</name>
    <title>Counting Semaphores</title>
    <filename>group__etcpal__sem.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SEM_HAS_TIMED_WAIT</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>gaada2b485228c5c702f240abd8d42c836</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SEM_HAS_POST_FROM_ISR</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>ga297a9999fd042d5dc38a1d4956c1cb50</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SEM_HAS_MAX_COUNT</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>ga6c5d81a112726041fbdc0836eeb86896</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETCPAL_SEM_MUST_BE_BALANCED</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>ga30a5fee3bf1f93fdb589fe2a99055b0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>PLATFORM_DEFINED</type>
      <name>etcpal_sem_t</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>ga90664f75ae59f03b85b45cc19d3a55ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_sem_create</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>ga5f8de05c22efb1e25bf24890b4afb21d</anchor>
      <arglist>(etcpal_sem_t *id, unsigned int initial_count, unsigned int max_count)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_sem_wait</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>gadf3cdb9d9d92dfde1b9933d6076e63aa</anchor>
      <arglist>(etcpal_sem_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_sem_try_wait</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>ga4e6901853d69d55f97717f14bb6eb290</anchor>
      <arglist>(etcpal_sem_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_sem_timed_wait</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>gad2f3af8fc26a2d028edd4c1341304061</anchor>
      <arglist>(etcpal_sem_t *id, int timeout_ms)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_sem_post</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>ga9e39fbcf982a2d058c7b3f1c598d6aac</anchor>
      <arglist>(etcpal_sem_t *id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>etcpal_sem_post_from_isr</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>gacd9bdeefdee24e47f40090840665c4fb</anchor>
      <arglist>(etcpal_sem_t *id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>etcpal_sem_destroy</name>
      <anchorfile>group__etcpal__sem.html</anchorfile>
      <anchor>gaa4d8fce64dfa93c4c2f7bd08100ef8b0</anchor>
      <arglist>(etcpal_sem_t *id)</arglist>
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
    <name>building_etcpal</name>
    <title>Building EtcPal</title>
    <filename>building_etcpal</filename>
  </compound>
  <compound kind="page">
    <name>building_for_embedded</name>
    <title>Building EtcPal for an embedded target</title>
    <filename>building_for_embedded</filename>
  </compound>
  <compound kind="page">
    <name>interface_indexes</name>
    <title>Network Interface Indexes</title>
    <filename>interface_indexes</filename>
  </compound>
  <compound kind="page">
    <name>targeting_freertos</name>
    <title>Targeting FreeRTOS</title>
    <filename>targeting_freertos</filename>
  </compound>
  <compound kind="page">
    <name>targeting_lwip</name>
    <title>Targeting lwIP</title>
    <filename>targeting_lwip</filename>
  </compound>
  <compound kind="page">
    <name>targeting_mqx</name>
    <title>Targeting MQX</title>
    <filename>targeting_mqx</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>ETC Platform Abstraction Layer</title>
    <filename>index</filename>
  </compound>
</tagfile>
