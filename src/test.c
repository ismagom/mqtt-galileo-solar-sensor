
/*
 * Copyright (c) 2013, Ismael Gomez - gomezi@tcd.ie
 * Based on https://github.com/menudoproblema/libemqtt
 */

/**
 * Reads A0, A1 and A2 sensors and publish MQTT messages
 */

#include <libemqtt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/tcp.h>

#include "ad.h"

#define RCVBUFSIZE 1024
uint8_t packet_buffer[RCVBUFSIZE];

int socket_id;


int send_packet(void* socket_info, const void* buf, unsigned int count)
{
	int fd = *((int*)socket_info);
	return send(fd, buf, count, 0);
}

int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port)
{
	int flag = 1;
	int keepalive = 3; // Seconds

	// Create the socket
	if((socket_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	// Disable Nagle Algorithm
	if (setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
		return -2;

	struct sockaddr_in socket_address;
	// Create the stuff we need to connect
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(port);
	socket_address.sin_addr.s_addr = inet_addr(hostname);

	// Connect the socket
	if((connect(socket_id, (struct sockaddr*)&socket_address, sizeof(socket_address))) < 0)
		return -1;

	// MQTT stuffs
	mqtt_set_alive(broker, keepalive);
	broker->socket_info = (void*)&socket_id;
	broker->send = send_packet;

	return 0;
}

int close_socket(mqtt_broker_handle_t* broker)
{
	int fd = *((int*)broker->socket_info);
	return close(fd);
}




int read_packet(int timeout)
{
	if(timeout > 0)
	{
		fd_set readfds;
		struct timeval tmv;

		// Initialize the file descriptor set
		FD_ZERO (&readfds);
		FD_SET (socket_id, &readfds);

		// Initialize the timeout data structure
		tmv.tv_sec = timeout;
		tmv.tv_usec = 0;

		// select returns 0 if timeout, 1 if input available, -1 if error
		if(select(1, &readfds, NULL, NULL, &tmv))
			return -2;
	}

	int total_bytes = 0, bytes_rcvd, packet_length;
	memset(packet_buffer, 0, sizeof(packet_buffer));

	while(total_bytes < 2) // Reading fixed header
	{
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}

	packet_length = packet_buffer[1] + 2; // Remaining length + fixed header length

	while(total_bytes < packet_length) // Reading the packet
	{
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}

	return packet_length;
}





const char* topics[3] = {"home/vbat", "home/solar", "home/resistor"};
int last_value[3];
const int threshold = 20;

int main(int argc, char* argv[])
{
	int n;
	int packet_length;
	uint16_t msg_id, msg_id_rcv;
	mqtt_broker_handle_t broker;

	if (argc<2) {
		printf("usage: %s mqtt_broker_host\n", argv[0]);
		return -1;
	}

	mqtt_init(&broker, "localhost");
	mqtt_init_auth(&broker, "ctvr", "ctvr");
	init_socket(&broker, argv[1], 1883);

	// >>>>> CONNECT
	mqtt_connect(&broker);
	// <<<<< CONNACK
	packet_length = read_packet(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) on read packet!\n", packet_length);
		return -1;
	}

	if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_CONNACK)
	{
		fprintf(stderr, "CONNACK expected!\n");
		return -2;
	}

	if(packet_buffer[3] != 0x00)
	{
		fprintf(stderr, "CONNACK failed!\n");
		return -2;
	}

	int i=0;
	uint64_t value;
	char msg[128];
	for (n=0;n<3;n++) {
		ad_init(n);
		last_value[n] = 0;
	}

	while(1) {
		for (n=0;n<3;n++) {
			value = TO_MILIVOLTS(ad_read(n));
			snprintf(msg, 128, "%d", value);
			mqtt_publish(&broker, topics[n], msg, 0);
		}
		sleep(2);
	}


	mqtt_disconnect(&broker);
	close_socket(&broker);
	return 0;
}

