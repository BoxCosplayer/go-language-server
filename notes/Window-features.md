# Window Features

## Show Message Notification
The `window/showMessage` notification lets the server show a pop-up message.
This is server to client and does not expect a response.

Notification:
- method: 'window/showMessage'
- params: ShowMessageParams

Interface:
```
export namespace MessageType {
	/**
	 * An error message.
	 */
	export const Error = 1;
	export const Warning = 2;
	export const Info = 3;
	export const Log = 4;
}

export type MessageType = 1 | 2 | 3 | 4;
```

Interface:
```
export interface ShowMessageParams {
	/**
	 * The message type.
	 */
	type: MessageType;

	/**
	 * The actual message.
	 */
	message: string;
}
```

## Show Message Request
The `window/showMessageRequest` request shows a message and asks the user to
choose an action.
The client returns the selected action item or `null`.

Request:
- method: 'window/showMessageRequest'
- params: ShowMessageRequestParams

Response:
- result: MessageActionItem | null
- error: code and message if an exception happens during the request

Client Capability:
- property path (optional): `window.showMessage.messageActionItem.additionalPropertiesSupport`
- property type: `boolean`

Interface:
```
export interface MessageActionItem {
	/**
	 * A short title like 'Retry' or 'Open Logs'.
	 */
	title: string;
}
```

Interface:
```
export interface ShowMessageRequestParams extends ShowMessageParams {
	/**
	 * The actions to present to the user.
	 */
	actions?: MessageActionItem[];
}
```

Interface:
```
export interface MessageActionItemClientCapabilities {
	/**
	 * Whether the client preserves additional properties on action items.
	 */
	additionalPropertiesSupport?: boolean;
}

export interface ShowMessageRequestClientCapabilities {
	messageActionItem?: MessageActionItemClientCapabilities;
}
```

## Log Message
The `window/logMessage` notification writes a message to the client's log.
It is typically less intrusive than `window/showMessage`.

Notification:
- method: 'window/logMessage'
- params: LogMessageParams

Interface:
```
export interface LogMessageParams {
	/**
	 * The message type.
	 */
	type: MessageType;

	/**
	 * The actual message.
	 */
	message: string;
}
```

## Show Document
The `window/showDocument` request asks the client to open a document, optionally
focusing it and selecting a range.
Clients advertise support via `window.showDocument`.

Request:
- method: 'window/showDocument'
- params: ShowDocumentParams

Response:
- result: ShowDocumentResult
- error: code and message if an exception happens during the request

Client Capability:
- property path (optional): `window.showDocument.support`
- property type: `boolean`

Interface:
```
export interface ShowDocumentParams {
	/**
	 * The document to open.
	 */
	uri: DocumentUri;

	/**
	 * Open the document outside the editor, for example in a browser.
	 */
	external?: boolean;

	/**
	 * Whether the editor should take focus.
	 */
	takeFocus?: boolean;

	/**
	 * An optional selection range once the document is opened.
	 */
	selection?: Range;
}
```

Interface:
```
export interface ShowDocumentResult {
	/**
	 * Indicates whether the document was shown.
	 */
	success: boolean;
}
```

Interface:
```
export interface ShowDocumentClientCapabilities {
	/**
	 * The client supports the `window/showDocument` request.
	 */
	support?: boolean;
}
```

## Create Work Done Progress
The `window/workDoneProgress/create` request creates a progress token on the client.
This is mainly used for server-initiated progress where the token did not come
from a client request.
After creation, progress updates are sent via the `$/progress` notification.

Request:
- method: 'window/workDoneProgress/create'
- params: WorkDoneProgressCreateParams

Response:
- result: null
- error: code and message if an exception happens during the request

Client Capability:
- property path (optional): `window.workDoneProgress`
- property type: `boolean`

Interface:
```
export type ProgressToken = integer | string;

export interface WorkDoneProgressCreateParams {
	/**
	 * The progress token to create.
	 */
	token: ProgressToken;
}
```

## Cancel Work Done Progress
The `window/workDoneProgress/cancel` notification tells the server to cancel
work associated with a progress token.
Cancellation is best effort and the server should stop reporting progress for
that token once cancelled.

Notification:
- method: 'window/workDoneProgress/cancel'
- params: WorkDoneProgressCancelParams

Interface:
```
export interface WorkDoneProgressCancelParams {
	/**
	 * The token to cancel.
	 */
	token: ProgressToken;
}
```

## Sent Telemetry
The `telemetry/event` notification sends telemetry data from the server to the client.
Clients are free to ignore telemetry events.

Notification:
- method: 'telemetry/event'
- params: LSPAny

Interface:
```
export interface WindowClientCapabilities {
	/**
	 * Whether the client supports server initiated progress via
	 * `window/workDoneProgress/create`.
	 */
	workDoneProgress?: boolean;

	/**
	 * Capabilities for `window/showMessageRequest`.
	 */
	showMessage?: ShowMessageRequestClientCapabilities;

	/**
	 * Capabilities for `window/showDocument`.
	 */
	showDocument?: ShowDocumentClientCapabilities;
}
```
