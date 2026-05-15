#ifndef BLINK_LORA_EEPROM_H
#define BLINK_LORA_EEPROM_H

#define AT_COMMAND "AT\r\n"
#define AT_MODE "AT+MODE=LWOTAA\r\n"
#define AT_KEY "AT+KEY=APPKEY,d4c922f29b39383e1ea912858527010c\r\n"
#define AT_CLASS "AT+CLASS=A\r\n"
#define AT_PORT "AT+PORT=8\r\n"
#define AT_DR "AT+DR=5\r\n"
#define AT_JOIN "AT+JOIN\r\n"
#define AT_MSG "AT+MSG="

#define AT_CMD_ANSWER "OK"
#define MODE_CMD_ANSWER "+MODE:"
#define KEY_CMD_ANSWER "+KEY:"
#define CLASS_CMD_ANSWER "+CLASS:"
#define PORT_CMD_ANSWER "+PORT:"
#define DR_CMD_ANSWER "+DR:"
#define JOIN_CMD_ANSWER "+JOIN: Network joined"
#define MSG_CMD_ANSWER "+MSG: Done"


#define CMD_TIMEOUTS1 500
#define CMD_TIMEOUTS2 20000

#define UART_ID uart1
#define MAX_COMMAND_LENGTH 50

int connect_lora();
bool check_response(const char *buffer, const char *expected);
bool send_lora_command(const char *cmd, const char *expected, int timeout);
bool send_message(bool *connection, const char *message);

#endif //BLINK_LORA_EEPROM_H