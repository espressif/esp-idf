
#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/wlanif.h"

#include <stdio.h>
#include <string.h>

#include "tcpip_adapter.h"

static struct netif *g_last_netif = NULL;


err_t nettestif_output(struct netif *netif, struct pbuf *p)
{
  int i;
  char *dat = p->payload;

  /* output the packet to stdout */
  printf("\nPacketOut:[");
  for (i=0; i<p->len; i++) {
    printf("%02x", *dat++);
  }
  printf("]\n");

  return ERR_OK;
}


err_t nettestif_init(struct netif *netif)
{

  g_last_netif = netif;

  netif->hostname = CONFIG_LWIP_LOCAL_HOSTNAME;

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100);

  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = nettestif_output;
   /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if ESP_LWIP
#if LWIP_IGMP
  netif->flags |= NETIF_FLAG_IGMP;
#endif
#endif
  return ERR_OK;

}

void nettestif_input(void *buffer, u16_t len)
{
  struct pbuf *p;
  if (g_last_netif == NULL) {
    printf("error!");
    return;
  }

  printf("simul in: %d\n", len);
  if (len==0) return;

  p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
  p->l2_owner = NULL;
  memcpy(p->payload, buffer, len);

  /* full packet send to tcpip_thread to process
   * on success - the packet is processed and deallocated in tcpip stack
   * on failure - log error and deallocate the packet
   */
  if (g_last_netif->input(p, g_last_netif) != ERR_OK) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
  }

}
