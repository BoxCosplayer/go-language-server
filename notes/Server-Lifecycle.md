# Server Lifecycle

## Overview
The lifecycle of a server is managed by the client; the client decides when to start a server process and when to shut it down.

## Initialise
The `initialize` request should be the first request sent from the client to the server.
If the server receives a request before `initialize`, it should respond with error code `-32002`.
Notifications should be dropped, except for the `exit` notification.
Until the server has responded with `InitializeResult`, the client must not send any other requests or notifications.
The server must not send any requests or notifications until it has replied, except it may send `window/showMessage`, `window/logMessage`, `telemetry/event`, and `window/showMessageRequest`.
If the client provides a progress token (for example `workDoneToken` in `InitializeParams`), the server may use that token with the `$/progress` notification during initialise.
The `initialize` request may only be sent once.

Request:
- method: 'initialize'
- params: InitializeParams

Interface:
```
interface InitializeParams extends WorkDoneProgressParams {
	// PID of the client, if the client dies, so should the server process 
	processId: integer | null;

	clientInfo?: {
		name: string;
		version?: string;
	};

	// IETF language tags
	locale?: string;

	// See below
	initializationOptions?: LSPAny;

	// List of Client Capabilities; see capabilities.md
	capabilities: ClientCapabilities;
	trace?: TraceValue;
	workspaceFolders?: WorkspaceFolder[] | null;
}
```

Response:
- result: InitializeResult
- error: code and message if an exception happens during initialise

Interface:
```
interface InitializeResult {
	capabilities: ServerCapabilities;

	serverInfo?: {
		name: string;
		version?: string;
	};
}
```

Errors and error codes

```
namespace InitializeErrorCodes {

	/**
	 * If the protocol version provided by the client can't be handled by
	 * the server.
	 *
	 * @deprecated This initialise error got replaced by client capabilities.
	 * There is no version handshake in version 3.0x
	 */
	export const unknownProtocolVersion: 1 = 1;
}

export type InitializeErrorCodes = 1;
```

## Initialised
The `initialized` notification is sent from the client to the server after the client has received the result of the `initialize` request, but before any other requests or notifications are sent.
The server can use this to dynamically register capabilities.
This notification may only be sent once.

Notification:
- method: 'initialized'
- params: InitializedParams

Interface:
```
interface InitializedParams {
}
```

## Register Capability
The `client/registerCapability` request is sent from the server to the client to register for a new capability on the client side.
Not all clients support dynamic capability registration; a client opts in via the `dynamicRegistration` property on the specific client capabilities.
A server must not register the same capability both statically (via the initialise result) and dynamically for the same document selector.
If a server wants to support both static and dynamic registration, it should check client capabilities and only register statically if dynamic registration is not supported.

Request:
- method: 'client/registerCapability'
- params: RegistrationParams

Interface:
```
export interface Registration {
	// The id used to register the request. The id can be used to deregister the request again.
	id: string;

	// The method / capability to register for.
	method: string;

	// Options necessary for the registration.
	registerOptions?: LSPAny;
}

export interface RegistrationParams {
	registrations: Registration[];
}
```

## Unregister Capability
The `client/unregisterCapability` request is sent from the server to the client to unregister a previously registered capability.

Request:
- method: 'client/unregisterCapability'
- params: UnregistrationParams

Interface:
```
export interface Unregistration {
	// The id used to unregister the request or notification. Usually an id provided during the register request.
	id: string;

	// The method / capability to unregister for.
	method: string;
}

export interface UnregistrationParams {
	// This should correctly be named `unregistrations`. However changing this
	// is a breaking change and needs to wait until we deliver a 4.x version
	// of the specification.
	unregisterations: Unregistration[];
}
```

## Set Trace
A notification that should be used by the client to modify the trace setting of the server.

Notification:
- method: '$/setTrace'
- params: SetTraceParams

Interface:
```
interface SetTraceParams {
	// The new value that should be assigned to the trace setting.
	value: TraceValue;
}
```

## Log Trace
A notification to log the trace of the server's execution.
The amount and content of these notifications depends on the current trace configuration.
If `trace` is 'off', the server should not send any `logTrace` notification.
If `trace` is 'messages', the server should not add the 'verbose' field in `LogTraceParams`.
`$/logTrace` should be used for systematic trace reporting.
For single debugging messages, the server should send `window/logMessage` notifications.

Notification:
- method: '$/logTrace'
- params: LogTraceParams

Interface:
```
interface LogTraceParams {
	// The message to be logged.
	message: string;

	// Additional information that can be computed if the trace configuration is set to 'verbose'
	verbose?: string;
}
```

## Shutdown
The `shutdown` request is sent from the client to the server.
It asks the server to shut down, but not exit (otherwise the response might not be delivered correctly to the client).
Clients must not send any notifications other than `exit` or requests after sending `shutdown`.
Clients should wait for the shutdown response before sending the `exit` notification.
If a server receives requests after `shutdown`, it should respond with `InvalidRequest`.

Request:
- method: 'shutdown'
- params: none

Response:
- result: null
- error: code and message if an exception happens during shutdown

## Exit
The `exit` notification asks the server to exit its process.
The server should exit with success code 0 if it received `shutdown` before; otherwise it should exit with error code 1.

Notification:
- method: 'exit'
- params: none
