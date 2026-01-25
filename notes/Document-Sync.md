# Document Synchronization

## Overview - Text Document
Client support for `textDocument/didOpen`, `textDocument/didChange`, and `textDocument/didClose` notifications is mandatory.
This includes both full and incremental synchronisation in `textDocument/didChange`.
A server must implement all three or none.
Opting out only makes sense for read-only documents.

Client Capability:
- property path (optional): `textDocument.synchronization.dynamicRegistration`
- property type: `boolean`

Server Capability:
- property path (optional): `textDocumentSync`
- property type: `TextDocumentSyncKind | TextDocumentSyncOptions`

Interface:
```
/**
 * Defines how the host (editor) should sync document changes to the language
 * server.
 */
export namespace TextDocumentSyncKind {
	/**
	 * Documents should not be synced at all.
	 */
	export const None = 0;

	/**
	 * Documents are synced by always sending the full content
	 * of the document.
	 */
	export const Full = 1;

	/**
	 * Documents are synced by sending the full content on open.
	 * After that only incremental updates to the document are
	 * sent.
	 */
	export const Incremental = 2;
}

export type TextDocumentSyncKind = 0 | 1 | 2;
```

Interface:
```
export interface TextDocumentSyncClientCapabilities {
	/**
	 * Whether text document synchronisation supports dynamic registration.
	 */
	dynamicRegistration?: boolean;

	/**
	 * The client supports sending will save notifications.
	 */
	willSave?: boolean;

	/**
	 * The client supports sending a will save request and
	 * waits for a response providing text edits which will
	 * be applied to the document before it is saved.
	 */
	willSaveWaitUntil?: boolean;

	/**
	 * The client supports did save notifications.
	 */
	didSave?: boolean;
}
```

Interface:
```
export interface TextDocumentSyncOptions {
	/**
	 * Open and close notifications are sent to the server. If omitted open
	 * close notification should not be sent.
	 */
	openClose?: boolean;
	/**
	 * Change notifications are sent to the server. See
	 * TextDocumentSyncKind.None, TextDocumentSyncKind.Full and
	 * TextDocumentSyncKind.Incremental. If omitted it defaults to
	 * TextDocumentSyncKind.None.
	 */
	change?: TextDocumentSyncKind;
	/**
	 * If present will save notifications are sent to the server. If omitted
	 * the notification should not be sent.
	 */
	willSave?: boolean;
	/**
	 * If present will save wait until requests are sent to the server. If
	 * omitted the request should not be sent.
	 */
	willSaveWaitUntil?: boolean;
	/**
	 * If present save notifications are sent to the server. If omitted the
	 * notification should not be sent.
	 */
	save?: boolean | SaveOptions;
}
```

Interface:
```
/**
 * General text document registration options.
 */
export interface TextDocumentRegistrationOptions {
	/**
	 * A document selector to identify the scope of the registration. If set to
	 * null the document selector provided on the client side will be used.
	 */
	documentSelector: DocumentSelector | null;
}
```

## Did Open Text Document
The `textDocument/didOpen` notification is sent when a document is opened.

Notification:
- method: 'textDocument/didOpen'
- params: DidOpenTextDocumentParams

Interface:
```
interface DidOpenTextDocumentParams {
	/**
	 * The document that was opened.
	 */
	textDocument: TextDocumentItem;
}
```

## Did Change Text Document
The `textDocument/didChange` notification is sent when the document changes.
Apply notifications in the order received, and apply each `TextDocumentContentChangeEvent` in order.

Notification:
- method: 'textDocument/didChange'
- params: DidChangeTextDocumentParams

Interface:
```
/**
 * Describe options to be used when registering for text document change events.
 */
export interface TextDocumentChangeRegistrationOptions
	extends TextDocumentRegistrationOptions {
	/**
	 * How documents are synced to the server. See TextDocumentSyncKind.Full
	 * and TextDocumentSyncKind.Incremental.
	 */
	syncKind: TextDocumentSyncKind;
}
```

Interface:
```
interface DidChangeTextDocumentParams {
	/**
	 * The document that did change. The version number points
	 * to the version after all provided content changes have
	 * been applied.
	 */
	textDocument: VersionedTextDocumentIdentifier;

	/**
	 * The actual content changes.
	 */
	contentChanges: TextDocumentContentChangeEvent[];
}
```

Interface:
```
/**
 * An event describing a change to a text document. If only a text is provided
 * it is considered to be the full content of the document.
 */
export type TextDocumentContentChangeEvent = {
	/**
	 * The range of the document that changed.
	 */
	range: Range;

	/**
	 * The optional length of the range that got replaced.
	 *
	 * @deprecated use range instead.
	 */
	rangeLength?: uinteger;

	/**
	 * The new text for the provided range.
	 */
	text: string;
} | {
	/**
	 * The new text of the whole document.
	 */
	text: string;
};
```

## Will Save Text Document
The `textDocument/willSave` notification is sent before a document is saved.

Notification:
- method: 'textDocument/willSave'
- params: WillSaveTextDocumentParams

Interface:
```
/**
 * The parameters send in a will save text document notification.
 */
export interface WillSaveTextDocumentParams {
	/**
	 * The document that will be saved.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The 'TextDocumentSaveReason'.
	 */
	reason: TextDocumentSaveReason;
}
```

Interface:
```
/**
 * Represents reasons why a text document is saved.
 */
export namespace TextDocumentSaveReason {

	/**
	 * Manually triggered, e.g. by the user pressing save, by starting
	 * debugging, or by an API call.
	 */
	export const Manual = 1;

	/**
	 * Automatic after a delay.
	 */
	export const AfterDelay = 2;

	/**
	 * When the editor lost focus.
	 */
	export const FocusOut = 3;
}

export type TextDocumentSaveReason = 1 | 2 | 3;
```

## Will Save Document Wait Until
The `textDocument/willSaveWaitUntil` request is sent before save to allow the server to return edits that will be applied before the save.

Request:
- method: 'textDocument/willSaveWaitUntil'
- params: WillSaveTextDocumentParams

Response:
- result: TextEdit[]
- error: code and message if an exception happens during the request

## Did Save Text Document
The `textDocument/didSave` notification is sent after a document is saved.
The `text` field is only present if the client was requested to include it.

Notification:
- method: 'textDocument/didSave'
- params: DidSaveTextDocumentParams

Interface:
```
export interface SaveOptions {
	/**
	 * The client is supposed to include the content on save.
	 */
	includeText?: boolean;
}
```

Interface:
```
export interface TextDocumentSaveRegistrationOptions
	extends TextDocumentRegistrationOptions {
	/**
	 * The client is supposed to include the content on save.
	 */
	includeText?: boolean;
}
```

Interface:
```
interface DidSaveTextDocumentParams {
	/**
	 * The document that was saved.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * Optional the content when saved. Depends on the includeText value
	 * when the save notification was requested.
	 */
	text?: string;
}
```

## Did Close Text Document
The `textDocument/didClose` notification is sent when a document is closed.

Notification:
- method: 'textDocument/didClose'
- params: DidCloseTextDocumentParams

Interface:
```
interface DidCloseTextDocumentParams {
	/**
	 * The document that was closed.
	 */
	textDocument: TextDocumentIdentifier;
}
```

## Rename a Text Document
Document renames should be signalled to a server by sending a `textDocument/didClose` notification for the old name, followed by a `textDocument/didOpen` notification for the new name.
This is because attributes like language can change, and the new document might not be relevant to the server.
Servers can participate in a document rename by subscribing to `workspace/didRenameFiles` or `workspace/willRenameFiles`.

## Overview - Notebook Document
Notebooks are modelled as a notebook document containing notebook cells.
Cell text is stored in regular text documents; cell text document URIs are opaque and must be unique across all notebook cells.
There are two sync modes:
- cellContent: only cell text documents are synced using `textDocument/did*`.
- notebook: notebook document, cells, and cell text content are synced using `notebookDocument/did*`.

In both modes, notebook cell text documents are always synchronised using incremental sync.

Interface:
```
/**
 * A notebook document.
 *
 * @since 3.17.0
 */
export interface NotebookDocument {

	/**
	 * The notebook document's URI.
	 */
	uri: URI;

	/**
	 * The type of the notebook.
	 */
	notebookType: string;

	/**
	 * The version number of this document (it will increase after each
	 * change, including undo/redo).
	 */
	version: integer;

	/**
	 * Additional metadata stored with the notebook
	 * document.
	 */
	metadata?: LSPObject;

	/**
	 * The cells of a notebook.
	 */
	cells: NotebookCell[];
}
```

Interface:
```
/**
 * A notebook cell.
 *
 * A cell's document URI must be unique across ALL notebook
 * cells and can therefore be used to uniquely identify a
 * notebook cell or the cell's text document.
 *
 * @since 3.17.0
 */
export interface NotebookCell {

	/**
	 * The cell's kind
	 */
	kind: NotebookCellKind;

	/**
	 * The URI of the cell's text document
	 * content.
	 */
	document: DocumentUri;

	/**
	 * Additional metadata stored with the cell.
	 */
	metadata?: LSPObject;

	/**
	 * Additional execution summary information
	 * if supported by the client.
	 */
	executionSummary?: ExecutionSummary;
}
```

Interface:
```
/**
 * A notebook cell kind.
 *
 * @since 3.17.0
 */
export namespace NotebookCellKind {

	/**
	 * A markup-cell is formatted source that is used for display.
	 */
	export const Markup: 1 = 1;

	/**
	 * A code-cell is source code.
	 */
	export const Code: 2 = 2;
}
```

Interface:
```
export interface ExecutionSummary {
	/**
	 * A strict monotonically increasing value
	 * indicating the execution order of a cell
	 * inside a notebook.
	 */
	executionOrder: uinteger;

	/**
	 * Whether the execution was successful or
	 * not if known by the client.
	 */
	success?: boolean;
}
```

Interface:
```
/**
 * A notebook cell text document filter denotes a cell text
 * document by different properties.
 *
 * @since 3.17.0
 */
export interface NotebookCellTextDocumentFilter {
	/**
	 * A filter that matches against the notebook
	 * containing the notebook cell. If a string
	 * value is provided it matches against the
	 * notebook type. '*' matches every notebook.
	 */
	notebook: string | NotebookDocumentFilter;

	/**
	 * A language id like `python`.
	 *
	 * Will be matched against the language id of the
	 * notebook cell document. '*' matches every language.
	 */
	language?: string;
}
```

Interface:
```
/**
 * A notebook document filter denotes a notebook document by
 * different properties.
 *
 * @since 3.17.0
 */
export type NotebookDocumentFilter = {
	/** The type of the enclosing notebook. */
	notebookType: string;

	/** A Uri scheme, like `file` or `untitled`. */
	scheme?: string;

	/** A glob pattern. */
	pattern?: string;
} | {
	/** The type of the enclosing notebook. */
	notebookType?: string;

	/** A Uri scheme, like `file` or `untitled`.*/
	scheme: string;

	/** A glob pattern. */
	pattern?: string;
} | {
	/** The type of the enclosing notebook. */
	notebookType?: string;

	/** A Uri scheme, like `file` or `untitled`. */
	scheme?: string;

	/** A glob pattern. */
	pattern: string;
};
```

Client Capability:
- property name (optional): `notebookDocument.synchronization`
- property type: `NotebookDocumentSyncClientCapabilities`

Interface:
```
/**
 * Notebook specific client capabilities.
 *
 * @since 3.17.0
 */
export interface NotebookDocumentSyncClientCapabilities {

	/**
	 * Whether implementation supports dynamic registration. If this is
	 * set to `true` the client supports the new
	 * `(NotebookDocumentSyncRegistrationOptions & NotebookDocumentSyncOptions)`
	 * return value for the corresponding server capability as well.
	 */
	dynamicRegistration?: boolean;

	/**
	 * The client supports sending execution summary data per cell.
	 */
	executionSummarySupport?: boolean;
}
```

Server Capability:
- property name (optional): `notebookDocumentSync`
- property type: `NotebookDocumentSyncOptions | NotebookDocumentSyncRegistrationOptions`

Interface:
```
/**
 * Options specific to a notebook plus its cells
 * to be synced to the server.
 *
 * If a selector provides a notebook document
 * filter but no cell selector all cells of a
 * matching notebook document will be synced.
 *
 * If a selector provides no notebook document
 * filter but only a cell selector all notebook
 * documents that contain at least one matching
 * cell will be synced.
 *
 * @since 3.17.0
 */
export interface NotebookDocumentSyncOptions {
	/**
	 * The notebooks to be synced
	 */
	notebookSelector: ({
		/**
		 * The notebook to be synced. If a string
		 * value is provided it matches against the
		 * notebook type. '*' matches every notebook.
		 */
		notebook: string | NotebookDocumentFilter;

		/**
		 * The cells of the matching notebook to be synced.
		 */
		cells?: { language: string }[];
	} | {
		/**
		 * The notebook to be synced. If a string
		 * value is provided it matches against the
		 * notebook type. '*' matches every notebook.
		 */
		notebook?: string | NotebookDocumentFilter;

		/**
		 * The cells of the matching notebook to be synced.
		 */
		cells: { language: string }[];
	})[];

	/**
	 * Whether save notification should be forwarded to
	 * the server. Will only be honoured if mode === `notebook`.
	 */
	save?: boolean;
}
```

Interface:
```
/**
 * Registration options specific to a notebook.
 *
 * @since 3.17.0
 */
export interface NotebookDocumentSyncRegistrationOptions extends
	NotebookDocumentSyncOptions, StaticRegistrationOptions {
}
```

## Did Open Notebook Document
The `notebookDocument/didOpen` notification is sent when a notebook document is opened.

Notification:
- method: 'notebookDocument/didOpen'
- params: DidOpenNotebookDocumentParams

Interface:
```
/**
 * The params sent in an open notebook document notification.
 *
 * @since 3.17.0
 */
export interface DidOpenNotebookDocumentParams {

	/**
	 * The notebook document that got opened.
	 */
	notebookDocument: NotebookDocument;

	/**
	 * The text documents that represent the content
	 * of a notebook cell.
	 */
	cellTextDocuments: TextDocumentItem[];
}
```

## Did Change Notebook Document
The `notebookDocument/didChange` notification is sent when the notebook changes.
Apply notifications in the order received.

Notification:
- method: 'notebookDocument/didChange'
- params: DidChangeNotebookDocumentParams

Interface:
```
/**
 * A versioned notebook document identifier.
 *
 * @since 3.17.0
 */
export interface VersionedNotebookDocumentIdentifier {

	/**
	 * The version number of this notebook document.
	 */
	version: integer;

	/**
	 * The notebook document's URI.
	 */
	uri: URI;
}
```

Interface:
```
/**
 * The params sent in a change notebook document notification.
 *
 * @since 3.17.0
 */
export interface DidChangeNotebookDocumentParams {

	/**
	 * The notebook document that did change. The version number points
	 * to the version after all provided changes have been applied.
	 */
	notebookDocument: VersionedNotebookDocumentIdentifier;

	/**
	 * The actual changes to the notebook document.
	 */
	change: NotebookDocumentChangeEvent;
}
```

Interface:
```
/**
 * A change event for a notebook document.
 *
 * @since 3.17.0
 */
export interface NotebookDocumentChangeEvent {
	/**
	 * The changed meta data if any.
	 */
	metadata?: LSPObject;

	/**
	 * Changes to cells
	 */
	cells?: {
		/**
		 * Changes to the cell structure to add or
		 * remove cells.
		 */
		structure?: {
			/**
			 * The change to the cell array.
			 */
			array: NotebookCellArrayChange;

			/**
			 * Additional opened cell text documents.
			 */
			didOpen?: TextDocumentItem[];

			/**
			 * Additional closed cell text documents.
			 */
			didClose?: TextDocumentIdentifier[];
		};

		/**
		 * Changes to notebook cells properties like its
		 * kind, execution summary or metadata.
		 */
		data?: NotebookCell[];

		/**
		 * Changes to the text content of notebook cells.
		 */
		textContent?: {
			document: VersionedTextDocumentIdentifier;
			changes: TextDocumentContentChangeEvent[];
		}[];
	};
}
```

Interface:
```
/**
 * A change describing how to move a `NotebookCell`
 * array from state S to S'.
 *
 * @since 3.17.0
 */
export interface NotebookCellArrayChange {
	/**
	 * The start offset of the cell that changed.
	 */
	start: uinteger;

	/**
	 * The deleted cells
	 */
	deleteCount: uinteger;

	/**
	 * The new cells, if any
	 */
	cells?: NotebookCell[];
}
```

## Did Save Notebook Document
The `notebookDocument/didSave` notification is sent when a notebook document is saved.

Notification:
- method: 'notebookDocument/didSave'
- params: DidSaveNotebookDocumentParams

Interface:
```
/**
 * A literal to identify a notebook document in the client.
 *
 * @since 3.17.0
 */
export interface NotebookDocumentIdentifier {
	/**
	 * The notebook document's URI.
	 */
	uri: URI;
}
```

Interface:
```
/**
 * The params sent in a save notebook document notification.
 *
 * @since 3.17.0
 */
export interface DidSaveNotebookDocumentParams {
	/**
	 * The notebook document that got saved.
	 */
	notebookDocument: NotebookDocumentIdentifier;
}
```

## Did Close Notebook Document
The `notebookDocument/didClose` notification is sent when a notebook document is closed.

Notification:
- method: 'notebookDocument/didClose'
- params: DidCloseNotebookDocumentParams

Interface:
```
/**
 * The params sent in a close notebook document notification.
 *
 * @since 3.17.0
 */
export interface DidCloseNotebookDocumentParams {

	/**
	 * The notebook document that got closed.
	 */
	notebookDocument: NotebookDocumentIdentifier;

	/**
	 * The text documents that represent the content
	 * of a notebook cell that got closed.
	 */
	cellTextDocuments: TextDocumentIdentifier[];
}
```
