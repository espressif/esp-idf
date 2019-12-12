/* Link WiFi library .wifi0iram sections to IRAM
   if this snippet is included */
*libnet80211.a:( .wifirxiram  .wifirxiram.*)
*libpp.a:( .wifirxiram  .wifirxiram.*)
