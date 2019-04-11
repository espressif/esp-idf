/* Link WiFi library .wifi0iram sections to IRAM
   if this snippet is included */
*libnet80211.a:( .wifi0iram  .wifi0iram.*)
*libpp.a:( .wifi0iram  .wifi0iram.*)
