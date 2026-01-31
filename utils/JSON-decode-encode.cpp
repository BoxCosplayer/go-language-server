// JSON deserialisation (decoding a message)

// Messages can be 1 of 3 types:

// Requests:
// json {
//     "jsonprc": "2.0",
//     "id": int
//     "method": string // "TypeOfTask/Task"
//     "params": {
//         ...
//     }
// }

// Notifications:
// json {
//     "jsonprc": "2.0",
//     "method": string // "TypeOfTask/Task"
//     "params": {
//         ...
//     }
// }

// Batches:
// json {
//     [message1, message2, message3, ...]
// }

// The id can be memoised right away
// The method defines the params (amount and type)

// JSON serialisation (encoding a response)
