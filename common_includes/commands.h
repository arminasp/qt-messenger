#ifndef COMMANDS_H
#define COMMANDS_H

// socket password
#define PASSWORD "123456789"

// system commands
#define GOOD "true"
#define BAD "false"
#define SYSTEM_MESSAGE_START '$'

// must begin with SYSTEM_MESSAGE_START
#define VALIDATION "$connect"
#define SIGNUP "$signup"
#define DUPLICATE "$duplicate"
#define LOG "$log_request"
#define UPDATE_LOG "$update_log"
#define LOG_FINISH "$log_finished"
#define CONNECTED "$connected"
#define DISCONNECTED "$disconnected"
#define IS_ONLINE "$is_online"
#define ADD_CONTACT "$add_contact"
#define ERROR "$error"
#define ALL "$all"
#define ID "$id"
#define USER "$user"
#define EXIT "$exit"


// send message or photo
#define SEND "send"
#define PHOTO "photo"
#define FILE "file"
// end

#endif
