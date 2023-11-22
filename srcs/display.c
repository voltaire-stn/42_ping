#include "ping_functions.h"

void    display_ping_init(t_data *dt)
{
    printf("PING %s (%s): %lu data bytes", dt->input_dest, dt->resolved_address, sizeof(dt->crafted_icmp.payload));
    if (dt->options_params.v)
        printf(", id 0x%x = %d", 4808, 4808); // TO DO
    printf("\n");
}

void    display_ping_OK(t_data *dt)
{
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", dt->one_seq.bytes, dt->resolved_address, dt->one_seq.icmp_seq_count, dt->one_seq.ttl, (float)dt->one_seq.time / 1000);
}

void    display_ping_unreachable(t_data *dt)
{
    char src_addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(dt->resolved_address), src_addr_str, INET_ADDRSTRLEN);

    printf("%d bytes from %s: Destination Host Unreachable\n", dt->one_seq.bytes, dt->resolved_address);
    debug_packet(&dt->one_seq.final_packet);
    if (dt->options_params.v)
    {
        const unsigned char *bytes = (const unsigned char *)dt->one_seq.final_packet.icmp;
        struct iphdr    *ip = dt->one_seq.final_packet.ip;
        struct icmphdr  *icmp = dt->one_seq.final_packet.icmp;
        struct icmphdr  *icmp_in_payload = (struct icmphdr *)(bytes + IP_HEADER_LEN + ICMP_HEADER_LEN);
        printf("IP Hdr Dump:\n");
        for (int i = 0; i < IP_HEADER_LEN / 2; i++)
            printf(" %02X%02X", bytes[i], bytes[i + 1]);
        printf("\n");
        printf("%2s %2s %3s %4s %4s %3s %4s %3s %3s %4s %8s %8s %s\n", "Vr","HL","TOS","Len","ID","Flg","off","TTL","Pro","cks","Src","Dst","Data");
        printf("%2x %2x %3x %4x %4x %3x %04x %3x %3x %4x %08x %08x\n", ip->version, ip->ihl, ip->tos, ip->tot_len, ip->id, 0x2, ip->frag_off, ip->ttl, ip->protocol, ip->check, ip->saddr, ip->daddr); // TO DO addresses as ascii and flag and reverse order
        printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04d", icmp->type, icmp->code, dt->one_seq.bytes, icmp_in_payload->un.echo.id, icmp_in_payload->un.echo.sequence);
    }
    printf("\n");
}

void    display_ping_end_stats(t_data *dt)
{
    int             ratio;
    // int             time_sum;
    struct timeval  final_tv;
    char            err[512];

    // printf(C_G_RED"TIMES:"C_RES"\n");
    // ft_lst_iter_content(dt->end_stats.times, debug_time);
    if (gettimeofday(&final_tv, &dt->tz) != 0)
        exit_error("time error: Cannot retrieve time\n");
    ratio = 100 - ((dt->end_stats.recv_nb * 100) / dt->end_stats.sent_nb);
    // time_sum = (final_tv.tv_sec - dt->init_tv.tv_sec) * 1000000 + final_tv.tv_usec - dt->init_tv.tv_usec;
    if (dt->end_stats.errors_nb <= 0)
        sprintf(err, "%s", "");
    else
        sprintf(err, "+%d errors, ", dt->end_stats.errors_nb);
    printf("--- %s ping statistics ---\n", dt->input_dest);
    printf("%d packets transmitted, %d received, %s%d%% packet loss\n", dt->end_stats.sent_nb, dt->end_stats.recv_nb, err, ratio);
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", (float)get_time_min(dt) / 1000, (float)get_time_avg(dt) / 1000, (float)get_time_max(dt) / 1000, (float)get_time_std(dt) / 1000);
}
