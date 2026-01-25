# Protocol Info

LSP spec 3.17.x - https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

what the input looks like:
```
(Headers)
Content-Length: (Number - Length in Bytes) 
\r\n
Content-Type: (String - mime type of the content, application, charset etc.)
\r\n
(second CRLF to signify end of headers)
\r\n
(Content as JSON-rpc http://www.jsonrpc.org/, default utf-8)
{
	"jsonrpc": "2.0",
	"id": 1,
	"method": "textDocument/completion",
	"params": {
		...
	}
}
```

## Different types of messages

Base Class
```
Interface Message {
	jsonrpc: string; # Always "2.0"
}
```

### Requests:
```
Interface RequestMessage extends Message {
	request_id: integer | string;
	method: string;
	params?: array | object;
}
```

### Responses:

Default Response:
If there is an error, result should not be in response. Applies vice-versa.
```
Interface RequestMessage extends Message {
	result?: LSPAny;
	error?: ResponseError;
}
```

Error Response:
```
interface ResponseError {
    code: integer # lookup the spec for more details on the different errors and error codes
    message: string # description of the error
    data?: LSPAny
}
```

### Notification
Messages sent from the client that dont expect a response.
Think event triggers, etc.

```
Interface NotificationMessage extends Message {
    method: String;
    params?: array | Object;
}
```

### Cancellation Support
More info about how to handle and treat these requsts @ the docs.
For a Response, the server should give a special error for this.

```
interface CancelParams {
    id: integer | string;
}
```

## Request/Response Ordering
Responses should be sent back to the client in the order that the requests are recieved in a queue.
Parallel processing could occur, but ensure that ultimate correctness is ensured
What will I do? To keep it simple, linear responses should be the intial aim until the features are all implemented first.

## Work Done Progress
Reporting Progress comes in three structures:

- Work Done Progress Begin
- ~~ Report
- ~~ End

### Work Done Progress Begin
To start progress reporting, this should be sent first.

WDPB Interface:
```
interface WorkDoneProgressBegin {
    kind: 'begin';
    title: string;
    cancellable?: boolean;

    // More detailed message.
    message?: string;

    // 1, 100 refers to 1%, 100%
    // This should always be 0 when this type of notification is sent.
    percentage?: uinteger;
}
```

### Work Done Progress Report
Same as the above interface, but without the title.
Used when a task as part of a job is finished, or when a job is finished

### Work Done Progress End
After the last Report has been sent, this should be sent.

```
interface WorkDoneProgressEnd {
	kind: 'end';
	// Indicates the outcome of the operation.
	message?: string;
}
```

### Work Done Progress Parameters
A token should be added onto a request from the client to signal to the server that it accepts reporting.
This can then be used as part of a response to signal what work is being reported.

Params Interface:
```
interface WorkDoneProgressParams {
	workDoneToken?: ProgressToken;
}
```

A progress reponse may look like this:

```
{
	"token": "1d546990-40a3-4b77-b134-46622995f6ae",
	"value": {
		"kind": "begin",
		"title": "Finding references for A#foo",
		"cancellable": false,
		"message": "Processing file X.ts",
		"percentage": 0
	}
}
```
NOTE: The presence of the token dictates its necessity rather than a specific capability.
Therefore, the server should check whether or not there is a token to reference when reporting progress.

To resolve issues where the client expects reporting but the server does not report progress,
special `Provider`-type capabilities must be advertised by the server. 

## Trace Value
This value represents the level of verbosity for the server's logs, as dictated by the client.

`type TraceValue = 'off' | 'messages' | 'verbose';`
