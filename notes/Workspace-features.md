# Workspace Features

## Workspace Symbols
The `workspace/symbol` request searches for symbols across the whole workspace.
Clients usually drive this from a global symbol search UI.
The result can be either `WorkspaceSymbol[]` (preferred) or `SymbolInformation[]`.

Request:
- method: 'workspace/symbol'
- params: WorkspaceSymbolParams

Response:
- result: WorkspaceSymbol[] | SymbolInformation[] | null
- error: code and message if an exception happens during the request

Client Capability:
- property path (optional): `workspace.symbol`
- property type: `WorkspaceSymbolClientCapabilities`

Server Capability:
- property path (optional): `workspaceSymbolProvider`
- property type: `boolean | WorkspaceSymbolOptions`

Interface:
```
export interface WorkspaceSymbolParams extends WorkDoneProgressParams, PartialResultParams {
	/**
	 * A query string to filter symbols by.
	 */
	query: string;
}
```

Interface:
```
export interface WorkspaceSymbol {
	/**
	 * The name of this symbol.
	 */
	name: string;

	/**
	 * The kind of this symbol.
	 */
	kind: SymbolKind;

	/**
	 * Tags for this symbol.
	 */
	tags?: SymbolTag[];

	/**
	 * Additional detail for the symbol, for example the signature.
	 */
	detail?: string;

	/**
	 * The location of the symbol. In 3.17 this can be a full `Location` or
	 * just a URI if the range is expensive to compute up front.
	 */
	location: WorkspaceSymbolLocation;

	/**
	 * The name of the symbol containing this symbol.
	 */
	containerName?: string;

	/**
	 * A data entry field that is preserved between requests and resolves.
	 */
	data?: LSPAny;
}

export type WorkspaceSymbolLocation = Location | { uri: DocumentUri };
```

Interface:
```
export interface WorkspaceSymbolClientCapabilities {
	/**
	 * Whether workspace symbol supports dynamic registration.
	 */
	dynamicRegistration?: boolean;

	/**
	 * Specific symbol kinds the client supports.
	 */
	symbolKind?: {
		valueSet?: SymbolKind[];
	};

	/**
	 * The client supports tags on symbols.
	 */
	tagSupport?: {
		valueSet: SymbolTag[];
	};

	/**
	 * The client supports resolving selected properties lazily.
	 */
	resolveSupport?: {
		properties: string[];
	};
}
```

Interface:
```
export interface WorkspaceSymbolOptions extends WorkDoneProgressOptions {
	/**
	 * The server supports `workspaceSymbol/resolve`.
	 */
	resolveProvider?: boolean;
}
```

## Workspace Symbol Resolve
The `workspaceSymbol/resolve` request lets the server fill in expensive fields
for a symbol that was returned earlier by `workspace/symbol`.
It is only meaningful when both the client advertises `resolveSupport` and the
server advertises `resolveProvider`.

Request:
- method: 'workspaceSymbol/resolve'
- params: WorkspaceSymbol

Response:
- result: WorkspaceSymbol
- error: code and message if an exception happens during the request

## Get Configuration
The `workspace/configuration` request asks the client for configuration values.
This is a server to client request and is guarded by the client capability
`workspace.configuration`.
Each item can scope the request to a document or workspace folder and select a
configuration section.

Request:
- method: 'workspace/configuration'
- params: ConfigurationParams

Response:
- result: LSPAny[]
- error: code and message if an exception happens during the request

Client Capability:
- property path (optional): `workspace.configuration`
- property type: `boolean`

Interface:
```
export interface ConfigurationParams {
	items: ConfigurationItem[];
}

export interface ConfigurationItem {
	/**
	 * The scope to get the configuration section for.
	 */
	scopeUri?: DocumentUri;

	/**
	 * The configuration section asked for.
	 */
	section?: string;
}
```

## Did Change Configuration
The `workspace/didChangeConfiguration` notification informs the server that
configuration has changed.
Clients can either send the whole settings object or rely on the server to
pull specific sections via `workspace/configuration`.

Notification:
- method: 'workspace/didChangeConfiguration'
- params: DidChangeConfigurationParams

Client Capability:
- property path (optional): `workspace.didChangeConfiguration.dynamicRegistration`
- property type: `boolean`

Registration (dynamic):
- request: `client/registerCapability`
- method: 'workspace/didChangeConfiguration'
- register options: DidChangeConfigurationRegistrationOptions

Interface:
```
export interface DidChangeConfigurationParams {
	/**
	 * The actual changed settings.
	 */
	settings: LSPAny;
}
```

Interface:
```
export interface DidChangeConfigurationClientCapabilities {
	/**
	 * Whether configuration change supports dynamic registration.
	 */
	dynamicRegistration?: boolean;
}
```

Interface:
```
export interface DidChangeConfigurationRegistrationOptions {
	/**
	 * The configuration sections to report changes for.
	 */
	section?: string | string[];
}
```

## Workspace Folders
The `workspace/workspaceFolders` request retrieves the list of open workspace
folders from the client.
Servers should check the client capability `workspace.workspaceFolders` before
using this request.

Request:
- method: 'workspace/workspaceFolders'
- params: none

Response:
- result: WorkspaceFolder[] | null
- error: code and message if an exception happens during the request

Client Capability:
- property path (optional): `workspace.workspaceFolders`
- property type: `boolean`

Server Capability:
- property path (optional): `workspace.workspaceFolders`
- property type: `WorkspaceFoldersServerCapabilities`

Interface:
```
export interface WorkspaceFolder {
	/**
	 * The associated URI for this workspace folder.
	 */
	uri: DocumentUri;

	/**
	 * The name of the workspace folder.
	 */
	name: string;
}
```

Interface:
```
export interface WorkspaceFoldersServerCapabilities {
	/**
	 * The server has support for workspace folders.
	 */
	supported?: boolean;

	/**
	 * Whether the server wants to receive workspace folder change notifications.
	 * If a string is provided it is used as the registration id.
	 */
	changeNotifications?: string | boolean;
}
```

## Did Change Workspace Folders
The `workspace/didChangeWorkspaceFolders` notification is sent when workspace
folders are added or removed.
This notification is tied to `WorkspaceFoldersServerCapabilities.changeNotifications`.

Notification:
- method: 'workspace/didChangeWorkspaceFolders'
- params: DidChangeWorkspaceFoldersParams

Interface:
```
export interface DidChangeWorkspaceFoldersParams {
	/**
	 * The actual workspace folder change event.
	 */
	event: WorkspaceFoldersChangeEvent;
}

export interface WorkspaceFoldersChangeEvent {
	/**
	 * Added workspace folders.
	 */
	added: WorkspaceFolder[];

	/**
	 * Removed workspace folders.
	 */
	removed: WorkspaceFolder[];
}
```

## Will Create Files
The `workspace/willCreateFiles` request is sent before files are created.
It allows the server to return a `WorkspaceEdit` that keeps references and
generated code in sync.
Clients may drop results if the server responds too slowly.

Request:
- method: 'workspace/willCreateFiles'
- params: CreateFilesParams

Response:
- result: WorkspaceEdit | null
- error: code and message if an exception happens during the request

Client Capability:
- property path (optional): `workspace.fileOperations`
- property type: `FileOperationsClientCapabilities`

Server Capability:
- property path (optional): `workspace.fileOperations`
- property type: `FileOperationOptions`

Interface:
```
export interface FileOperationsClientCapabilities {
	/**
	 * Whether file operations support dynamic registration.
	 */
	dynamicRegistration?: boolean;
	didCreate?: boolean;
	willCreate?: boolean;
	didRename?: boolean;
	willRename?: boolean;
	didDelete?: boolean;
	willDelete?: boolean;
}
```

Interface:
```
export interface FileOperationOptions {
	didCreate?: FileOperationRegistrationOptions;
	willCreate?: FileOperationRegistrationOptions;
	didRename?: FileOperationRegistrationOptions;
	willRename?: FileOperationRegistrationOptions;
	didDelete?: FileOperationRegistrationOptions;
	willDelete?: FileOperationRegistrationOptions;
}

export interface FileOperationRegistrationOptions {
	/**
	 * The actual filters.
	 */
	filters: FileOperationFilter[];
}

export interface FileOperationFilter {
	/**
	 * A URI scheme like `file` or `untitled`.
	 */
	scheme?: string;
	pattern: FileOperationPattern;
}

export interface FileOperationPattern {
	/**
	 * The glob pattern to match.
	 */
	glob: string;

	/**
	 * Whether the pattern matches files, folders, or both.
	 */
	matches?: FileOperationPatternKind;

	/**
	 * Additional options for the glob.
	 */
	options?: FileOperationPatternOptions;
}

export namespace FileOperationPatternKind {
	export const File = 'file';
	export const Folder = 'folder';
}

export type FileOperationPatternKind = 'file' | 'folder';

export interface FileOperationPatternOptions {
	ignoreCase?: boolean;
}
```

Interface:
```
export interface CreateFilesParams {
	files: FileCreate[];
}

export interface FileCreate {
	uri: DocumentUri;
}
```

## Did Create Files
The `workspace/didCreateFiles` notification is sent after files are created.

Notification:
- method: 'workspace/didCreateFiles'
- params: CreateFilesParams

## Will Rename Files
The `workspace/willRenameFiles` request runs before files or folders are renamed.
It is commonly used to update imports or references.

Request:
- method: 'workspace/willRenameFiles'
- params: RenameFilesParams

Response:
- result: WorkspaceEdit | null
- error: code and message if an exception happens during the request

Interface:
```
export interface RenameFilesParams {
	files: FileRename[];
}

export interface FileRename {
	oldUri: DocumentUri;
	newUri: DocumentUri;
}
```

## Did Rename Files
The `workspace/didRenameFiles` notification is sent after files or folders are renamed.

Notification:
- method: 'workspace/didRenameFiles'
- params: RenameFilesParams

## Will Delete Files
The `workspace/willDeleteFiles` request runs before files or folders are deleted.
It allows the server to clean up generated code or update references.

Request:
- method: 'workspace/willDeleteFiles'
- params: DeleteFilesParams

Response:
- result: WorkspaceEdit | null
- error: code and message if an exception happens during the request

Interface:
```
export interface DeleteFilesParams {
	files: FileDelete[];
}

export interface FileDelete {
	uri: DocumentUri;
}
```

## Did Delete Files
The `workspace/didDeleteFiles` notification is sent after files or folders are deleted.

Notification:
- method: 'workspace/didDeleteFiles'
- params: DeleteFilesParams

## Did Change Watched Files
The `workspace/didChangeWatchedFiles` notification reports file system changes
that match server registered watchers.
In 3.17 the client can support relative patterns to reduce duplication.

Notification:
- method: 'workspace/didChangeWatchedFiles'
- params: DidChangeWatchedFilesParams

Client Capability:
- property path (optional): `workspace.didChangeWatchedFiles`
- property type: `DidChangeWatchedFilesClientCapabilities`

Registration (dynamic):
- request: `client/registerCapability`
- method: 'workspace/didChangeWatchedFiles'
- register options: DidChangeWatchedFilesRegistrationOptions

Interface:
```
export interface DidChangeWatchedFilesClientCapabilities {
	/**
	 * Whether watched files support dynamic registration.
	 */
	dynamicRegistration?: boolean;

	/**
	 * The client supports relative glob patterns.
	 */
	relativePatternSupport?: boolean;
}
```

Interface:
```
export interface DidChangeWatchedFilesParams {
	changes: FileEvent[];
}

export interface FileEvent {
	uri: DocumentUri;
	type: FileChangeType;
}

export namespace FileChangeType {
	export const Created = 1;
	export const Changed = 2;
	export const Deleted = 3;
}

export type FileChangeType = 1 | 2 | 3;
```

Interface:
```
export interface DidChangeWatchedFilesRegistrationOptions {
	watchers: FileSystemWatcher[];
}

export interface FileSystemWatcher {
	/**
	 * The glob pattern to watch.
	 */
	globPattern: GlobPattern;

	/**
	 * The kind of changes to report. If omitted all are reported.
	 */
	kind?: WatchKind;
}

export type GlobPattern = string | RelativePattern;

export interface RelativePattern {
	/**
	 * A workspace folder or URI to resolve the pattern against.
	 */
	baseUri: WorkspaceFolder | DocumentUri;

	/**
	 * The actual glob pattern.
	 */
	pattern: string;
}

export namespace WatchKind {
	export const Create = 1;
	export const Change = 2;
	export const Delete = 4;
}

export type WatchKind = number;
```

## Execute Command
The `workspace/executeCommand` request asks the server to run a command.
Commands must be declared in `executeCommandProvider.commands`.

Request:
- method: 'workspace/executeCommand'
- params: ExecuteCommandParams

Response:
- result: LSPAny
- error: code and message if an exception happens during the request

Client Capability:
- property path (optional): `workspace.executeCommand.dynamicRegistration`
- property type: `boolean`

Server Capability:
- property path (optional): `executeCommandProvider`
- property type: `ExecuteCommandOptions`

Interface:
```
export interface ExecuteCommandParams extends WorkDoneProgressParams {
	command: string;
	arguments?: LSPAny[];
}
```

Interface:
```
export interface ExecuteCommandOptions extends WorkDoneProgressOptions {
	commands: string[];
}

export interface ExecuteCommandRegistrationOptions extends ExecuteCommandOptions {}
```

## Apply Edit
The `workspace/applyEdit` request is sent from the server to the client to
apply a `WorkspaceEdit`.
Clients report whether the edit was applied and can provide a failure reason.

Request:
- method: 'workspace/applyEdit'
- params: ApplyWorkspaceEditParams

Response:
- result: ApplyWorkspaceEditResult
- error: code and message if an exception happens during the request

Interface:
```
export interface ApplyWorkspaceEditParams {
	/**
	 * An optional label for the edit.
	 */
	label?: string;
	edit: WorkspaceEdit;
}

export interface ApplyWorkspaceEditResult {
	/**
	 * Indicates whether the edit was applied.
	 */
	applied: boolean;

	/**
	 * An optional reason for a failure.
	 */
	failureReason?: string;

	/**
	 * The index of the change that failed.
	 */
	failedChange?: uinteger;
}
```
