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

## Capabilities
What features _should_ a language server support?
The list of features that an editor/server can handle is queried using a special capabilities request/response.

Need to read more to gain a somewhat-intimate knowledge of the features that I want to add.
In theory, I could cherrypick only the features I want to implement into GOatpad, and add them to both editor and server at the same time.
Therefore, I will list all the features I want in README.md over time.

## Request/Response Ordering
Responses should be sent back to the client in the order that the requests are recieved in a queue.
Parallel processing could occur, but ensure that ultimate correctness is ensured
What will I do? To keep it simple, linear responses should be the intial aim until the features are all implemented first.

# JSON Structures
Some special JSON terms need to be used for communication between Client/Server.

## URI
URI format is formally defined here: https://tools.ietf.org/html/rfc3986

URI example:
```
  foo://example.com:8042/over/there?name=ferret#nose
  \_/   \______________/\_________/ \_________/ \__/
   |           |            |            |        |
scheme     authority       path        query   fragment
   |   _____________________|__
  / \ /                        \
  urn:example:animal:ferret:nose
``` 
(https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#uri)

Being able to decode a URI into its individual parts (as above) is necessary.

## Regular Expressions
Regex isn't something I personally want to deal with yet but maybe later.
Notes will be unaffected.

Client/server supports x regex engines, which should be negotiated with the server which one to use - see capabilities
Specific feature negotiation see - ECMAScript Regular Expression specification (https://tc39.es/ecma262/#sec-regexp-regular-expression-objects)

## Enumerations
Talk to someone smarter than me. 
I dont yet understand just from looking at docs tbh.

docsLink/#enumerations

## Text Documents
Goatpad uses UTF-8 encoding as of now, which is great since on versions 3.17+ clients and servers can agree on different encodings (via capabilities) other than UTF-16. Support for UTF-16 may be added afterthefact, since this LS will be built for goatpad, but with standardastions in mind it should be thought of.
Newline position should also be kept in mind (LF vs CRLF, etc..)

## Position
https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#position
