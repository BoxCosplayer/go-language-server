These are my notes for the Language Server Protocol, dictated by Microsoft at the link below. This is really just for personal use / research and not indicitive of the projetcs features or scale - for that, read the readme as well as any other associated docs.
This will still be helpful for those who are trying to learn more about LSPs through this project, as my notes are a less formal lens of the spec itself.
If you have any questions about these notes - or spot something is incomplete for the version I have updated this to, please reach out to me via rajveer@sandhuhome.uk.


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
The position is the location of a cursor that is in an insert block state.
0-indexed line number and 0-indexed character position

Interface:
```
Interface Position {
    line: uinteger;
    character: uinteger
}
```
NOTE: character should be clamped to the EOL, in the case that it is larger than the line length

[PositionEncodingKind] == the capability(?) representing bit encoding of the string (e.g. utf-8, utf-16 etc..)
Definetly something that should be negotiated beforehand, though.

## Range
A range of characters, made of two positions.
See above.

The start should be Inclusive, whilst the end should be Exclusive.
For example, if a client wanted to send a request involving a newline character, the end position should represent the first character on the following line.

Interface:
```
Interface Range {
    start: {line: uinteger, character: uinteger};
    end: {line: uinteger, character: uinteger};
}
Interface Range {
    start: Position;
    end: Position;
}
```

## TextDocumentItem
"An item to transfer a text document from the client to the server."
Best described by its documented Interface.

Interface:
```
interface TextDocumentItem {
    uri: DocumentUri;
    LanguageId: string;
    version: integer;
    text: string;
}
```
NOTE: 'version' should increment upwards to account fo reahc change, "including undo/redo".

The Language id for each language type is usually the extension name for that language.
For exmaple, the id for c++ should be cpp. 

Sometimes, this is not the case.
For example, python maps to python instead of py, and powershell maps to poewrshell instead of ps1.

In case of any doubt, consult the documentation at https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#textDocumentItem

## TextDocumentIdentifier
This is just a reference to a document's URI specifically, omitting metdata about the Item itself, such as version and language details etc.

Interface:
```
interface TextDocumentIdentifier {
    uri: DocumentUri;
}
```

## VersionedTextDocumentIdentifier
Same as above but for file version.

Interface:
```
interface VersionedTextDocumentIdentifier extends TextDocumentIdentifier {
    version: integer;
}
```

There is also an Optional VTDI, with the same interface implementation as above, with use cases below.

## TextDocumentPositionParams
A structure that combines a TDI with a position

Interface:
```
interface TextDocumentPositionParams {
    textDocument: TextDocumentIdentifier;
    position: Position;
}
```
NOTE: there isn't an equivalent for range... its something to think about.
One would assume from this limited definition that this is only to be used as a response from the server.
Maybe. Needs more market research perhaps.

## DocumentFilter
Splits Documents via their three main components:
- language
- scheme
- pattern

Interface:
```
export interface DocumentFilter {
	languageId?: string;

	/**
	 * A Uri scheme, like `file` or `untitled`.
	 */
	scheme?: string;

	/**
	 * A glob pattern, like `*.{ts,js}`.
	 *
	 * Glob patterns can have the following syntax:
	 * - `*` to match zero or more characters in a path segment
	 * - `?` to match on one character in a path segment
	 * - `**` to match any number of path segments, including none
	 * - `{}` to group sub patterns into an OR expression. (e.g. `**​/*.{ts,js}`
	 *   matches all TypeScript and JavaScript files)
	 * - `[]` to declare a range of characters to match in a path segment
	 *   (e.g., `example.[0-9]` to match on `example.0`, `example.1`, …)
	 * - `[!...]` to negate a range of characters to match in a path segment
	 *   (e.g., `example.[!0-9]` to match on `example.a`, `example.b`, but
	 *   not `example.0`)
	 */
	pattern?: string;
}

```

Filter examples:
```
{ language: 'typescript', scheme: 'file' }
{ language: 'json', pattern: '**/package.json' }
```

At least one of the optional attributes must be set to ensure a valid filter.

## DocumentSelector
A DocumentSelector is simply a collection of Filters, as so:
`export type DocumentSelector = DocumentFilter[];`

## TextEdit
This gives the client a way to indicate that a text edit has occured on the file
// Should this increment the version of the file on ther server-side??

Interface:
```
interface TextEdit {
    range: Range;
    newText: string;
}
```

## ChangeAnnotation
Annotation can be changed (or added, see below) with ChangeAnnotations.

Interface:
```
export interface ChangeAnnotation {
	label: string;

	/**
	 * A flag which indicates that user confirmation is needed
	 * before applying the change.
	 */
	needsConfirmation?: boolean;
	description?: string;
}
```

This also ships with an identifier:
`export type ChangeAnnotationIdentifier = string;`

## AnnotatedTextEdit
A special TextEdit with a change annotation attached.
Useful when the client can group or label edits in the UI.

Interface:
```
interface AnnotatedTextEdit extends TextEdit {
    annotationId: ChangeAnnotationIdentifier;
}
```

## TextEdit[]
Docs put it best here.
"Complex text manipulations are described with an array of TextEdits / AnnotatedTextEdits"
TextEdit[] ranges must never overlap - so before it is generated, any given set of changes must be compiled or 'squashed'

## TextDocumentEdit
Links TextEdits to Documents via the aforementioned OVTDI.

Interface:
```
interface TextDocumentEdit {
    textDocument: OptionalVersionedTextDocumentIdentifier;
    edits: (TextEdit | AnnotatedTextEdit)[];
}
```

## Location
Represents a range inside a resource

Interface:
```
interface Location {
    uri: DocumentUri;
    range: Range;
}
```

## LocationLink
Represents a link between a source and a target location.
Useful for features that allow jumping between different files, or even different parts of a large file. (e.g. refactoring, definition seeking etc.)

Interface:
```
interface LocationLink {
    // Range of the source file that triggered the LocationLink
    originSelectionRange?: Range;
    
    targetUri: DocumentUri;

    // The "full" target range of the link
    targetRange: Range;
    // The range that the UI should focus on
    targetSelectionRange: Range;
}
```
the difference between targetRange and `targetSelectionRange` is that `targetRange` spans the whole selection - think of the whole range of a definition of a symbol, etc.. 
And the `targetSelectionRange` is only the bit that the UI should display - think the first line of the definition itself. 

## Diagnostic
This represents errors, warnings, hints, info etc..
The diagnostics, if you will. 
    - Captain Obvious

Interface:
```
interface Diagnostic {
    range: Range;
    severity?: DiagnosticSeverity;

    // Error code
    code?: integer | string;
    codeDescription?: CodeDescription

    // where did this diagnostic come from
    source?: string;
    // what did the diagnostic say
    message: string;
    
    // tags the diagnostic referred to
    tags?: DiagnosticTag[];

    // All references needed to understand what the diagnostic is
    relatedInformation?: DiagnosticRelatedInformation[];

    // See below
    data?: LSPAny;
}
```
Not sure specifically what 'data' is meant to be encoding here. The docs state:
A data entry field that is preserved between a
	 `textDocument/publishDiagnostics` notification and
	 `textDocument/codeAction` request.
Interesting.
An LLM output states:
> what types of things does the data field typically store here?

> Typical Diagnostic.data contents are tool-specific blobs the server wants back later, like:

>   - A rule id or analyzer name (e.g., { rule: "unused-var" })
>   - Enough context to build a quick fix without re-analyzing (e.g., symbol id, range, suggested replacement)
>   - IDs for external systems (e.g., linter issue id, compiler error code)
>   - Serialized hints for codeAction resolution (e.g., { fixId: "apply-import", target: "fmt" })
>   - There’s no standard schema; it’s “whatever your server needs to reconstruct or resolve the diagnostic later.”

Cool. For now, this is another thing that I will update when I learn more later, either from an example and/or someone much more knowledgable than I.

### More Diagnostic Structures
This section will detail out some of the substructures used above. Relatively self-explanatory.

DiagnosticSeverity namespace:
```
export namespace DiagnosticSeverity {
	const Error: 1 = 1;
	const Warning: 2 = 2;
	const Information: 3 = 3;
	const Hint: 4 = 4;
}

export type DiagnosticSeverity = 1 | 2 | 3 | 4;
```

DiagnosticTag namespace:
```
namespace DiagnosticTag {
    // Ive never seen this before but maybe that means im an amazing programmer
    // Jokes aside, need to do some research into what actually triggers this tag
    const Unnecessary: 1 = 1;
    const Deprecated: 2 = 2;
}

export type DiagnosticTag = 1 | 2;
```

DiagnosticRelatedInformation Interface:
```
interface DiagnosticRelatedInformation {
    location: location;
    message: string;
}
```

CodeDescription Interface:
```
interface CodeDescription {
    // Why it's called href here is actually beyond me
    // there MUST be some historical context here or im just stupid and its just a link to docs referring 
    // to the error in more detail
    href: URI;
}
```

## Command
This represents a standard way of trnasmitting commands. As of now, there is no list of recommended / standardised commands which is neat since I can just mimic and implement features of other IDEs supposing I can handle input/output in the JSON RPC Interface form.

Command Interface:
```
interface Command {
    title: string;
    command: string;

    // s/e
    arguments?: LSPAny[]
}
```
The title is the name of the command, in human-friendly text.
The `command` string is the command that is to be run.
(e.g. Organize Imports | editor.action.organizeImports)

## Markup Content
This represents a string value, that dictates whether to encode text as plaintext or as markdown.

MarkupKind namespace:
```
namespace MarkupKind {
    const Plaintext: 'plaintext' = 'plaintext';
    const Markdown: 'markdown' = 'markdown';
}
export type MarkupKind = 'plaintext' | 'markdown';
```
Why not use bools to toggle between the two? I do not know YET.

MarkupKind Interface:
```
interface MarkupContent {
    kind: MarkupKind;
    value: string
}
```

## File Resource Changes
These allow servers to create, rename and delete files & folders on behalf of the client.
Note that this appiles to both files and folders.

CreateFileOptions Interface (separate interface)
```
interface CreateFileOptions {
    overwrite?: boolean;
    ignoreIfExists?: boolean;
}
```
Note that `overwrite` bool always wins over `ignoreIfExists`.

CreateFile Interface
```
interface CreateFileOptions {
    kind: 'create';
    uri: DocumentUri;
    
    // See above
    options?: CreateFileOptions;
    annotationId?: ChangeAnnotationIdentifier;
}
```

RenameFileOptions Interface
```
interface RenameFileOptions {
    overwrite?: boolean;
    ignoreIfExists?: boolean;
}
```

RenameFile Interface
```
interface RenameFile {
    kind: 'rename';
    oldUri: DocumentUri;
    newUri: DocumentUri;
    
    // See above
    options?: CreateFileOptions;
    annotationId?: ChangeAnnotationIdentifier;
}
```

DeleteFileOptions Interface (separate interface)
```
interface DeleteFileOptions {
    recursive?: boolean;
    ignoreIfNotExists?: boolean;
}
```

DeleteFile Interface
```
interface DeleteFileOptions {
    kind: 'delete';
    uri: DocumentUri;
    
    // See above
    options?: CreateFileOptions;
    annotationId?: ChangeAnnotationIdentifier;
}
```

## WorkspaceEdit
This represents an edit that applies ot multiple different resources simultaneously.

WorkspaceEdit Interface
```
interface WorkspaceEdit {
    changes?: { [uri: DocumentUri]: TextEdit[]; };

    // Necessity of these two parameters are dictated by capabilities
    documentChanges?: (
        TextDocumentEdit[] |
        (TextDocumentEdit | CreateFile | RenameFile | DeleteFile)[]
    );

    changeAnnotations?: {
        [changeAnnotationIdentifier: string]: ChangeAnnotation;
    }
}
```

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

# Server Lifecycle
