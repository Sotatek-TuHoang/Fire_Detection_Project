#ifndef MESSAGE_H
#define MESSAGE_H

#define QUEUE_SIZE 16

typedef enum
{
  MSG_MODULE_SIM,
  MSG_SENSOR_SHT3X,
  MSG_SENSOR_SMOKE,
  MSG_QUE_NUM
} MsgQue;

typedef enum
{
  TASK_MODULE_SIM,
  TASK_SENSOR_SHT3X,
  TASK_SENSOR_SMOKE,
  TASK_NUM
} Sender;

typedef struct
{
  Sender sender; /**< The sender of the message. */
  void *p_msg;   /**< Pointer to the original (untyped) message being sent. */
} Message;

typedef enum
{
  E_NOT_PROCESSED = -1,
  E_SUCCESS = 0,
  E_MODULE_SIM,
  E_SENSOR_SHT3X,
  E_SENSOR_SMOKE,
  E_ENQUEUE_FAILED,
  E_MAX
} Error;

typedef struct
{
  uint8_t data;
} SensorSht3xMSG;

typedef struct
{
  uint8_t data;
} ModuleSimMSG;

typedef struct
{
  uint8_t data;
} SensorSmokeMSG;

/**
 * @brief API initialize queue contain messages.
 * @param[out] logic The byte to send.
 */
int msg_que_init();

/**
 * @brief API send internal message.
 * @param[in] dst Value indicating type queue message
 * @param[in] sender Value indicating type sender
 * @param[in] data Pointer to void indicating data in internal message
 * @return Error code
 */
Error msg_que_send(MsgQue dst, Sender sender, void *data);

#endif // MESSAGE_H