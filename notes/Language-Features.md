# Language Features

## Overview
Language features are optional and negotiated via client and server capabilities.
Most methods are client to server requests that return a result or `null`.
Many features support resolve or refresh requests for lazy loading and invalidation.

## Go to Declaration
The `textDocument/declaration` request asks for the declaration(s) of the symbol at a given position.

Request:
- method: 'textDocument/declaration'
- params: DeclarationParams

Response:
- result: Declaration (Location | Location[] | LocationLink[] | null)
- error: code and message if an exception happens during the request

## Go to Definition
The `textDocument/definition` request asks for the definition(s) of the symbol at a given position.

Request:
- method: 'textDocument/definition'
- params: DefinitionParams

Response:
- result: Definition (Location | Location[] | LocationLink[] | null)
- error: code and message if an exception happens during the request

## Go to Type Definition
The `textDocument/typeDefinition` request asks for the type definition(s) of the symbol at a given position.

Request:
- method: 'textDocument/typeDefinition'
- params: TypeDefinitionParams

Response:
- result: TypeDefinition (Location | Location[] | LocationLink[] | null)
- error: code and message if an exception happens during the request

## Go to Implementation
The `textDocument/implementation` request asks for implementation location(s) of the symbol at a given position.

Request:
- method: 'textDocument/implementation'
- params: ImplementationParams

Response:
- result: Implementation (Location | Location[] | LocationLink[] | null)
- error: code and message if an exception happens during the request

## Find References
The `textDocument/references` request returns references to the symbol at a given position.

Request:
- method: 'textDocument/references'
- params: ReferenceParams

Response:
- result: Location[] | null
- error: code and message if an exception happens during the request

Interface:
```
interface ReferenceContext {
	/**
	 * Include the declaration of the current symbol.
	 */
	includeDeclaration: boolean;
}
```

## Prepare Call Hierarchy
The `textDocument/prepareCallHierarchy` request returns the root items for call hierarchy.

Request:
- method: 'textDocument/prepareCallHierarchy'
- params: CallHierarchyPrepareParams

Response:
- result: CallHierarchyItem[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface CallHierarchyPrepareParams extends TextDocumentPositionParams,
	WorkDoneProgressParams {
}
```

Interface:
```
export interface CallHierarchyItem {
	/**
	 * The name of this item.
	 */
	name: string;

	/**
	 * The kind of this item.
	 */
	kind: SymbolKind;

	/**
	 * Tags for this item.
	 */
	tags?: SymbolTag[];

	/**
	 * More detail for this item, e.g. the signature of a function.
	 */
	detail?: string;

	/**
	 * The resource identifier of this item.
	 */
	uri: DocumentUri;

	/**
	 * The range enclosing this symbol not including leading/trailing whitespace
	 * but everything else, e.g. comments and code.
	 */
	range: Range;

	/**
	 * The range that should be selected and revealed when this symbol is being
	 * picked, e.g. the name of a function. Must be contained by the
	 * [`range`](#CallHierarchyItem.range).
	 */
	selectionRange: Range;

	/**
	 * A data entry field that is preserved between a call hierarchy prepare and
	 * incoming calls or outgoing calls requests.
	 */
	data?: LSPAny;
}
```

## Call Hierarchy Incoming Calls
The `callHierarchy/incomingCalls` request returns callers for a prepared call hierarchy item.

Request:
- method: 'callHierarchy/incomingCalls'
- params: CallHierarchyIncomingCallsParams

Response:
- result: CallHierarchyIncomingCall[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface CallHierarchyIncomingCallsParams extends
	WorkDoneProgressParams, PartialResultParams {
	item: CallHierarchyItem;
}
```

Interface:
```
export interface CallHierarchyIncomingCall {

	/**
	 * The item that makes the call.
	 */
	from: CallHierarchyItem;

	/**
	 * The ranges at which the calls appear. This is relative to the caller
	 * denoted by [`this.from`](#CallHierarchyIncomingCall.from).
	 */
	fromRanges: Range[];
}
```

## Call Hierarchy Outgoing Calls
The `callHierarchy/outgoingCalls` request returns calls made by a prepared call hierarchy item.

Request:
- method: 'callHierarchy/outgoingCalls'
- params: CallHierarchyOutgoingCallsParams

Response:
- result: CallHierarchyOutgoingCall[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface CallHierarchyOutgoingCallsParams extends
	WorkDoneProgressParams, PartialResultParams {
	item: CallHierarchyItem;
}
```

Interface:
```
export interface CallHierarchyOutgoingCall {

	/**
	 * The item that is called.
	 */
	to: CallHierarchyItem;

	/**
	 * The range at which this item is called. This is the range relative to
	 * the caller, e.g the item passed to `callHierarchy/outgoingCalls` request.
	 */
	fromRanges: Range[];
}
```

## Prepare Type Hierarchy
The `textDocument/prepareTypeHierarchy` request returns the root items for type hierarchy.

Request:
- method: 'textDocument/prepareTypeHierarchy'
- params: TypeHierarchyPrepareParams

Response:
- result: TypeHierarchyItem[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface TypeHierarchyPrepareParams extends TextDocumentPositionParams,
	WorkDoneProgressParams {
}
```

Interface:
```
export interface TypeHierarchyItem {
	/**
	 * The name of this item.
	 */
	name: string;

	/**
	 * The kind of this item.
	 */
	kind: SymbolKind;

	/**
	 * Tags for this item.
	 */
	tags?: SymbolTag[];

	/**
	 * More detail for this item, e.g. the signature of a function.
	 */
	detail?: string;

	/**
	 * The resource identifier of this item.
	 */
	uri: DocumentUri;

	/**
	 * The range enclosing this symbol not including leading/trailing whitespace
	 * but everything else, e.g. comments and code.
	 */
	range: Range;

	/**
	 * The range that should be selected and revealed when this symbol is being
	 * picked, e.g. the name of a function. Must be contained by the
	 * [`range`](#TypeHierarchyItem.range).
	 */
	selectionRange: Range;

	/**
	 * A data entry field that is preserved between a type hierarchy prepare and
	 * supertypes or subtypes requests. It could also be used to identify the
	 * type hierarchy in the server, helping improve the performance on
	 * resolving supertypes and subtypes.
	 */
	data?: LSPAny;
}
```

## Type Hierarchy Super Types
The `typeHierarchy/supertypes` request returns supertypes for a prepared type hierarchy item.

Request:
- method: 'typeHierarchy/supertypes'
- params: TypeHierarchySupertypesParams

Response:
- result: TypeHierarchyItem[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface TypeHierarchySupertypesParams extends
	WorkDoneProgressParams, PartialResultParams {
	item: TypeHierarchyItem;
}
```

## Type Hierarchy Sub Types
The `typeHierarchy/subtypes` request returns subtypes for a prepared type hierarchy item.

Request:
- method: 'typeHierarchy/subtypes'
- params: TypeHierarchySubtypesParams

Response:
- result: TypeHierarchyItem[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface TypeHierarchySubtypesParams extends
	WorkDoneProgressParams, PartialResultParams {
	item: TypeHierarchyItem;
}
```

## Document Highlight
The `textDocument/documentHighlight` request highlights related ranges at a position.

Request:
- method: 'textDocument/documentHighlight'
- params: DocumentHighlightParams

Response:
- result: DocumentHighlight[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface DocumentHighlightParams extends TextDocumentPositionParams,
	WorkDoneProgressParams, PartialResultParams {
}
```

Interface:
```
export interface DocumentHighlight {
	/**
	 * The range this highlight applies to.
	 */
	range: Range;

	/**
	 * The highlight kind, default is DocumentHighlightKind.Text.
	 */
	kind?: DocumentHighlightKind;
}
```

Interface:
```
export namespace DocumentHighlightKind {
	/**
	 * A textual occurrence.
	 */
	export const Text = 1;

	/**
	 * Read-access of a symbol, like reading a variable.
	 */
	export const Read = 2;

	/**
	 * Write-access of a symbol, like writing to a variable.
	 */
	export const Write = 3;
}

export type DocumentHighlightKind = 1 | 2 | 3;
```

## Document Link
The `textDocument/documentLink` request returns links inside a document.

Request:
- method: 'textDocument/documentLink'
- params: DocumentLinkParams

Response:
- result: DocumentLink[] | null
- error: code and message if an exception happens during the request

Interface:
```
interface DocumentLinkParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The document to provide document links for.
	 */
	textDocument: TextDocumentIdentifier;
}
```

Interface:
```
interface DocumentLink {
	/**
	 * The range this link applies to.
	 */
	range: Range;

	/**
	 * The uri this link points to. If missing a resolve request is sent later.
	 */
	target?: URI;

	/**
	 * The tooltip text when you hover over this link.
	 *
	 * If a tooltip is provided, is will be displayed in a string that includes
	 * instructions on how to trigger the link, such as `{0} (ctrl + click)`.
	 * The specific instructions vary depending on OS, user settings, and
	 * localisation.
	 *
	 * @since 3.15.0
	 */
	tooltip?: string;

	/**
	 * A data entry field that is preserved on a document link between a
	 * DocumentLinkRequest and a DocumentLinkResolveRequest.
	 */
	data?: LSPAny;
}
```

## Document Link Resolve
The `documentLink/resolve` request resolves additional data for a link.

Request:
- method: 'documentLink/resolve'
- params: DocumentLink

Response:
- result: DocumentLink
- error: code and message if an exception happens during the request

## Hover
The `textDocument/hover` request returns hover information at a position.

Request:
- method: 'textDocument/hover'
- params: HoverParams

Response:
- result: Hover | null
- error: code and message if an exception happens during the request

Interface:
```
export interface HoverParams extends TextDocumentPositionParams,
	WorkDoneProgressParams {
}
```

Interface:
```
export interface Hover {
	/**
	 * The hover's content
	 */
	contents: MarkedString | MarkedString[] | MarkupContent;

	/**
	 * An optional range is a range inside a text document
	 * that is used to visualise a hover, e.g. by changing the background colour.
	 */
	range?: Range;
}
```

## Code Lens
The `textDocument/codeLens` request returns code lens items for a document.

Request:
- method: 'textDocument/codeLens'
- params: CodeLensParams

Response:
- result: CodeLens[] | null
- error: code and message if an exception happens during the request

Interface:
```
interface CodeLensParams extends WorkDoneProgressParams, PartialResultParams {
	/**
	 * The document to request code lens for.
	 */
	textDocument: TextDocumentIdentifier;
}
```

Interface:
```
interface CodeLens {
	/**
	 * The range in which this code lens is valid. Should only span a single
	 * line.
	 */
	range: Range;

	/**
	 * The command this code lens represents.
	 */
	command?: Command;

	/**
	 * A data entry field that is preserved on a code lens item between
	 * a code lens and a code lens resolve request.
	 */
	data?: LSPAny;
}
```

## Code Lens Refresh
The `workspace/codeLens/refresh` request asks the client to re-request code lenses.

Request:
- method: 'workspace/codeLens/refresh'
- params: none

Response:
- result: null
- error: code and message if an exception happens during the request

## Folding Range
The `textDocument/foldingRange` request returns folding ranges for a document.

Request:
- method: 'textDocument/foldingRange'
- params: FoldingRangeParams

Response:
- result: FoldingRange[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface FoldingRangeParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;
}
```

Interface:
```
export interface FoldingRange {

	/**
	 * The zero-based start line of the range to fold. The folded area starts
	 * after the line's last character. To be valid, the end must be zero or
	 * larger and smaller than the number of lines in the document.
	 */
	startLine: uinteger;

	/**
	 * The zero-based character offset from where the folded range starts. If
	 * not defined, defaults to the length of the start line.
	 */
	startCharacter?: uinteger;

	/**
	 * The zero-based end line of the range to fold. The folded area ends with
	 * the line's last character. To be valid, the end must be zero or larger
	 * and smaller than the number of lines in the document.
	 */
	endLine: uinteger;

	/**
	 * The zero-based character offset before the folded range ends. If not
	 * defined, defaults to the length of the end line.
	 */
	endCharacter?: uinteger;

	/**
	 * Describes the kind of the folding range such as `comment` or `region`.
	 * The kind is used to categorise folding ranges and used by commands like
	 * 'Fold all comments'. See [FoldingRangeKind](#FoldingRangeKind) for an
	 * enumeration of standardised kinds.
	 */
	kind?: FoldingRangeKind;

	/**
	 * The text that the client should show when the specified range is
	 * collapsed. If not defined or not supported by the client, a default
	 * will be chosen by the client.
	 *
	 * @since 3.17.0 - proposed
	 */
	collapsedText?: string;
}
```

Interface:
```
export namespace FoldingRangeKind {
	/**
	 * Folding range for a comment
	 */
	export const Comment = 'comment';

	/**
	 * Folding range for imports or includes
	 */
	export const Imports = 'imports';

	/**
	 * Folding range for a region (e.g. `#region`)
	 */
	export const Region = 'region';
}

/**
 * The type is a string since the value set is extensible
 */
export type FoldingRangeKind = string;
```

## Selection Range
The `textDocument/selectionRange` request returns nested selection ranges.

Request:
- method: 'textDocument/selectionRange'
- params: SelectionRangeParams

Response:
- result: SelectionRange[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface SelectionRangeParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The positions inside the text document.
	 */
	positions: Position[];
}
```

Interface:
```
export interface SelectionRange {
	/**
	 * The [range](#Range) of this selection range.
	 */
	range: Range;
	/**
	 * The parent selection range containing this range. Therefore
	 * `parent.range` must contain `this.range`.
	 */
	parent?: SelectionRange;
}
```

## Document Symbols
The `textDocument/documentSymbol` request returns symbols for a document.

Request:
- method: 'textDocument/documentSymbol'
- params: DocumentSymbolParams

Response:
- result: DocumentSymbol[] | SymbolInformation[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface DocumentSymbolParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;
}
```

Interface:
```
export interface DocumentSymbol {

	/**
	 * The name of this symbol. Will be displayed in the user interface and
	 * therefore must not be an empty string or a string only consisting of
	 * white spaces.
	 */
	name: string;

	/**
	 * More detail for this symbol, e.g the signature of a function.
	 */
	detail?: string;

	/**
	 * The kind of this symbol.
	 */
	kind: SymbolKind;

	/**
	 * Tags for this document symbol.
	 *
	 * @since 3.16.0
	 */
	tags?: SymbolTag[];

	/**
	 * Indicates if this symbol is deprecated.
	 *
	 * @deprecated Use tags instead
	 */
	deprecated?: boolean;

	/**
	 * The range enclosing this symbol not including leading/trailing whitespace
	 * but everything else like comments. This information is typically used to
	 * determine if the clients cursor is inside the symbol to reveal it  in the
	 * UI.
	 */
	range: Range;

	/**
	 * The range that should be selected and revealed when this symbol is being
	 * picked, e.g. the name of a function. Must be contained by the `range`.
	 */
	selectionRange: Range;

	/**
	 * Children of this symbol, e.g. properties of a class.
	 */
	children?: DocumentSymbol[];
}
```

Interface:
```
export interface SymbolInformation {
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
	 *
	 * @since 3.16.0
	 */
	tags?: SymbolTag[];

	/**
	 * Indicates if this symbol is deprecated.
	 *
	 * @deprecated Use tags instead
	 */
	deprecated?: boolean;

	/**
	 * The location of this symbol.
	 */
	location: Location;

	/**
	 * The name of the symbol containing this symbol.
	 */
	containerName?: string;
}
```

## Semantic Tokens
Semantic tokens provide lightweight semantic classification for tokens.
Tokens are encoded as integers in 5-tuples: deltaLine, deltaStart, length, tokenType, tokenModifiers.

Requests:
- method: 'textDocument/semanticTokens/full'
- params: SemanticTokensParams
- method: 'textDocument/semanticTokens/full/delta'
- params: SemanticTokensDeltaParams
- method: 'textDocument/semanticTokens/range'
- params: SemanticTokensRangeParams

Responses:
- result (full/range): SemanticTokens | null
- result (delta): SemanticTokensDelta | null
- error: code and message if an exception happens during the request

Interface:
```
export interface SemanticTokensParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;
}
```

Interface:
```
export interface SemanticTokensDeltaParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The result id of a previous response. The result Id can either point to
	 * a full response or a delta response depending on what was received last.
	 */
	previousResultId: string;
}
```

Interface:
```
export interface SemanticTokensRangeParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The range the semantic tokens are requested for.
	 */
	range: Range;
}
```

Interface:
```
export interface SemanticTokens {
	/**
	 * An optional result id. If provided and clients support delta updating
	 * the client will include the result id in the next semantic token request.
	 * A server can then instead of computing all semantic tokens again simply
	 * send a delta.
	 */
	resultId?: string;

	/**
	 * The actual tokens.
	 */
	data: uinteger[];
}
```

Interface:
```
export interface SemanticTokensDelta {
	readonly resultId?: string;
	/**
	 * The semantic token edits to transform a previous result into a new
	 * result.
	 */
	edits: SemanticTokensEdit[];
}
```

Interface:
```
export interface SemanticTokensEdit {
	/**
	 * The start offset of the edit.
	 */
	start: uinteger;

	/**
	 * The count of elements to remove.
	 */
	deleteCount: uinteger;

	/**
	 * The elements to insert.
	 */
	data?: uinteger[];
}
```

Interface:
```
export interface SemanticTokensLegend {
	/**
	 * The token types a server uses.
	 */
	tokenTypes: string[];

	/**
	 * The token modifiers a server uses.
	 */
	tokenModifiers: string[];
}
```

## Inline Value
The `textDocument/inlineValue` request provides inline values for debugging views.

Request:
- method: 'textDocument/inlineValue'
- params: InlineValueParams

Response:
- result: InlineValue[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface InlineValueParams extends WorkDoneProgressParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The document range for which inline values should be computed.
	 */
	range: Range;

	/**
	 * Additional information about the context in which inline values were
	 * requested.
	 */
	context: InlineValueContext;
}
```

Interface:
```
export interface InlineValueContext {
	/**
	 * The stack frame (as a DAP Id) where the execution has stopped.
	 */
	frameId: integer;

	/**
	 * The document range where execution has stopped.
	 * Typically the end position of the range denotes the line where the
	 * inline values are shown.
	 */
	stoppedLocation: Range;
}
```

Interface:
```
export interface InlineValueText {
	/**
	 * The document range for which the inline value applies.
	 */
	range: Range;

	/**
	 * The text of the inline value.
	 */
	text: string;
}
```

Interface:
```
export interface InlineValueVariableLookup {
	/**
	 * The document range for which the inline value applies.
	 * The range is used to extract the variable name from the underlying
	 * document.
	 */
	range: Range;

	/**
	 * If specified the name of the variable to look up.
	 */
	variableName?: string;

	/**
	 * How to perform the lookup.
	 */
	caseSensitiveLookup: boolean;
}
```

Interface:
```
export interface InlineValueEvaluatableExpression {
	/**
	 * The document range for which the inline value applies.
	 * The range is used to extract the evaluatable expression from the
	 * underlying document.
	 */
	range: Range;

	/**
	 * If specified the expression overrides the extracted expression.
	 */
	expression?: string;
}
```

Interface:
```
export type InlineValue = InlineValueText | InlineValueVariableLookup
	| InlineValueEvaluatableExpression;
```

## Inline Value Refresh
The `workspace/inlineValue/refresh` request asks the client to re-request inline values.

Request:
- method: 'workspace/inlineValue/refresh'
- params: none

Response:
- result: null
- error: code and message if an exception happens during the request

## Inlay Hint
The `textDocument/inlayHint` request returns inlay hints for a range.

Request:
- method: 'textDocument/inlayHint'
- params: InlayHintParams

Response:
- result: InlayHint[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface InlayHintParams extends WorkDoneProgressParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The visible document range for which inlay hints should be computed.
	 */
	range: Range;
}
```

Interface:
```
export interface InlayHint {

	/**
	 * The position of this hint.
	 *
	 * If multiple hints have the same position, they will be shown in the order
	 * they appear in the response.
	 */
	position: Position;

	/**
	 * The label of this hint. A human readable string or an array of
	 * InlayHintLabelPart label parts.
	 *
	 * *Note* that neither the string nor the label part can be empty.
	 */
	label: string | InlayHintLabelPart[];

	/**
	 * The kind of this hint. Can be omitted in which case the client
	 * should fall back to a reasonable default.
	 */
	kind?: InlayHintKind;

	/**
	 * Optional text edits that are performed when accepting this inlay hint.
	 *
	 * *Note* that edits are expected to change the document so that the inlay
	 * hint (or its nearest variant) is now part of the document and the inlay
	 * hint itself is now obsolete.
	 *
	 * Depending on the client capability `inlayHint.resolveSupport` clients
	 * might resolve this property late using the resolve request.
	 */
	textEdits?: TextEdit[];

	/**
	 * The tooltip text when you hover over this item.
	 *
	 * Depending on the client capability `inlayHint.resolveSupport` clients
	 * might resolve this property late using the resolve request.
	 */
	tooltip?: string | MarkupContent;

	/**
	 * Render padding before the hint.
	 *
	 * Note: Padding should use the editor's background colour, not the
	 * background colour of the hint itself. That means padding can be used
	 * to visually align/separate an inlay hint.
	 */
	paddingLeft?: boolean;

	/**
	 * Render padding after the hint.
	 *
	 * Note: Padding should use the editor's background colour, not the
	 * background colour of the hint itself. That means padding can be used
	 * to visually align/separate an inlay hint.
	 */
	paddingRight?: boolean;


	/**
	 * A data entry field that is preserved on an inlay hint between
	 * a `textDocument/inlayHint` and a `inlayHint/resolve` request.
	 */
	data?: LSPAny;
}
```

Interface:
```
export interface InlayHintLabelPart {

	/**
	 * The value of this label part.
	 */
	value: string;

	/**
	 * The tooltip text when you hover over this label part. Depending on
	 * the client capability `inlayHint.resolveSupport` clients might resolve
	 * this property late using the resolve request.
	 */
	tooltip?: string | MarkupContent;

	/**
	 * An optional source code location that represents this
	 * label part.
	 */
	location?: Location;

	/**
	 * An optional command for this label part.
	 */
	command?: Command;
}
```

Interface:
```
export namespace InlayHintKind {

	/**
	 * An inlay hint that for a type annotation.
	 */
	export const Type = 1;

	/**
	 * An inlay hint that is for a parameter.
	 */
	export const Parameter = 2;
}

export type InlayHintKind = 1 | 2;
```

## Inlay Hint Resolve
The `inlayHint/resolve` request resolves additional data for an inlay hint.

Request:
- method: 'inlayHint/resolve'
- params: InlayHint

Response:
- result: InlayHint
- error: code and message if an exception happens during the request

## Inlay Hint Refresh
The `workspace/inlayHint/refresh` request asks the client to re-request inlay hints.

Request:
- method: 'workspace/inlayHint/refresh'
- params: none

Response:
- result: null
- error: code and message if an exception happens during the request

## Moniker
The `textDocument/moniker` request returns monikers for the symbol at a position.

Request:
- method: 'textDocument/moniker'
- params: MonikerParams

Response:
- result: Moniker[] | null
- error: code and message if an exception happens during the request

Interface:
```
/**
 * Moniker definition to match LSIF 0.5 moniker definition.
 */
export interface Moniker {
	/**
	 * The scheme of the moniker. For example tsc or .Net
	 */
	scheme: string;

	/**
	 * The identifier of the moniker. The value is opaque in LSIF however
	 * schema owners are allowed to define the structure if they want.
	 */
	identifier: string;

	/**
	 * The scope in which the moniker is unique
	 */
	unique: UniquenessLevel;

	/**
	 * The moniker kind if known.
	 */
	kind?: MonikerKind;
}
```

## Completion Proposals
The `textDocument/completion` request returns completion items or lists.

Request:
- method: 'textDocument/completion'
- params: CompletionParams

Response:
- result: CompletionItem[] | CompletionList | null
- error: code and message if an exception happens during the request

Interface:
```
export interface CompletionParams extends TextDocumentPositionParams,
	WorkDoneProgressParams, PartialResultParams {
	/**
	 * The completion context. This is only available if the client specifies
	 * to send this using the client capability
	 * `completion.contextSupport === true`
	 */
	context?: CompletionContext;
}
```

## Completion Item Resolve
The `completionItem/resolve` request resolves additional information for a completion item.

Request:
- method: 'completionItem/resolve'
- params: CompletionItem

Response:
- result: CompletionItem
- error: code and message if an exception happens during the request

## Publish Diagnostics
The `textDocument/publishDiagnostics` notification publishes diagnostics for a document.

Notification:
- method: 'textDocument/publishDiagnostics'
- params: PublishDiagnosticsParams

Interface:
```
interface PublishDiagnosticsParams {
	/**
	 * The URI for which diagnostic information is reported.
	 */
	uri: DocumentUri;

	/**
	 * Optional the version number of the document the diagnostics are published
	 * for.
	 *
	 * @since 3.15.0
	 */
	version?: integer;

	/**
	 * An array of diagnostic information items.
	 */
diagnostics: Diagnostic[];
}
```

## Pull Diagnostics
Pull diagnostics uses requests to fetch diagnostics instead of push notifications.
Clients use `textDocument/diagnostic` for a single document and `workspace/diagnostic` for workspace-wide diagnostics.
Servers can request a refresh via `workspace/diagnostic/refresh`.

Request:
- method: 'textDocument/diagnostic'
- params: DocumentDiagnosticParams

Response:
- result: DocumentDiagnosticReport
- error: code and message if an exception happens during the request

Request:
- method: 'workspace/diagnostic'
- params: WorkspaceDiagnosticParams

Response:
- result: WorkspaceDiagnosticReport
- error: code and message if an exception happens during the request

Request:
- method: 'workspace/diagnostic/refresh'
- params: none

Response:
- result: null
- error: code and message if an exception happens during the request

Interface:
```
export interface DocumentDiagnosticParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The text document.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The additional identifier  provided during registration.
	 */
	identifier?: string;

	/**
	 * The result id of a previous response if provided.
	 */
	previousResultId?: string;
}
```

Interface:
```
export type DocumentDiagnosticReport = RelatedFullDocumentDiagnosticReport
	| RelatedUnchangedDocumentDiagnosticReport;
```

Interface:
```
export namespace DocumentDiagnosticReportKind {
	/**
	 * A diagnostic report with a full
	 * set of problems.
	 */
	export const Full = 'full';

	/**
	 * A report indicating that the last
	 * returned report is still accurate.
	 */
	export const Unchanged = 'unchanged';
}

export type DocumentDiagnosticReportKind = 'full' | 'unchanged';
```

Interface:
```
export interface FullDocumentDiagnosticReport {
	/**
	 * A full document diagnostic report.
	 */
	kind: DocumentDiagnosticReportKind.Full;

	/**
	 * An optional result id. If provided it will
	 * be sent on the next diagnostic request for the
	 * same document.
	 */
	resultId?: string;

	/**
	 * The actual items.
	 */
	items: Diagnostic[];
}
```

Interface:
```
export interface UnchangedDocumentDiagnosticReport {
	/**
	 * A document diagnostic report indicating
	 * no changes to the last result. A server can
	 * only return `unchanged` if result ids are
	 * provided.
	 */
	kind: DocumentDiagnosticReportKind.Unchanged;

	/**
	 * A result id which will be sent on the next
	 * diagnostic request for the same document.
	 */
	resultId: string;
}
```

Interface:
```
export interface WorkspaceDiagnosticParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The additional identifier provided during registration.
	 */
	identifier?: string;

	/**
	 * The currently known diagnostic reports with their
	 * previous result ids.
	 */
	previousResultIds: PreviousResultId[];
}
```

Interface:
```
export interface WorkspaceDiagnosticReport {
	items: WorkspaceDocumentDiagnosticReport[];
}
```

## Signature Help
The `textDocument/signatureHelp` request returns signature help for a position.

Request:
- method: 'textDocument/signatureHelp'
- params: SignatureHelpParams

Response:
- result: SignatureHelp | null
- error: code and message if an exception happens during the request

Interface:
```
export interface SignatureHelpParams extends TextDocumentPositionParams,
	WorkDoneProgressParams {
	/**
	 * The signature help context. This is only available if the client
	 * specifies to send this using the client capability
	 * `textDocument.signatureHelp.contextSupport === true`
	 *
	 * @since 3.15.0
	 */
	context?: SignatureHelpContext;
}
```

Interface:
```
export interface SignatureHelp {
	/**
	 * One or more signatures. If no signatures are available the signature help
	 * request should return `null`.
	 */
	signatures: SignatureInformation[];

	/**
	 * The active signature. If omitted or the value lies outside the
	 * range of `signatures` the value defaults to zero or is ignore if
	 * the `SignatureHelp` as no signatures.
	 */
	activeSignature?: uinteger;

	/**
	 * The active parameter of the active signature. If omitted or the value
	 * lies outside the range of `signatures[activeSignature].parameters`
	 * defaults to 0 if the active signature has parameters. If
	 * the active signature has no parameters it is ignored.
	 */
	activeParameter?: uinteger;
}
```

Interface:
```
export interface SignatureInformation {
	/**
	 * The label of this signature. Will be shown in
	 * the UI.
	 */
	label: string;

	/**
	 * The human-readable doc-comment of this signature. Will be shown
	 * in the UI but can be omitted.
	 */
	documentation?: string | MarkupContent;

	/**
	 * The parameters of this signature.
	 */
	parameters?: ParameterInformation[];

	/**
	 * The index of the active parameter.
	 */
	activeParameter?: uinteger;
}
```

Interface:
```
export interface ParameterInformation {

	/**
	 * The label of this parameter information.
	 */
	label: string | [uinteger, uinteger];

	/**
	 * The human-readable doc-comment of this parameter. Will be shown
	 * in the UI but can be omitted.
	 */
	documentation?: string | MarkupContent;
}
```

Interface:
```
export interface SignatureHelpContext {
	/**
	 * Action that caused signature help to be triggered.
	 */
	triggerKind: SignatureHelpTriggerKind;

	/**
	 * Character that caused signature help to be triggered.
	 */
	triggerCharacter?: string;

	/**
	 * `true` if signature help was already showing when it was triggered.
	 */
	isRetrigger: boolean;

	/**
	 * The currently active `SignatureHelp`.
	 */
activeSignatureHelp?: SignatureHelp;
}
```

## Code Action
The `textDocument/codeAction` request returns code actions or commands for a range.

Request:
- method: 'textDocument/codeAction'
- params: CodeActionParams

Response:
- result: (CodeAction | Command)[] | null
- error: code and message if an exception happens during the request

Interface:
```
export interface CodeActionParams extends WorkDoneProgressParams,
	PartialResultParams {
	/**
	 * The document in which the command was invoked.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The range for which the command was invoked.
	 */
	range: Range;

	/**
	 * Context carrying additional information.
	 */
	context: CodeActionContext;
}
```

Interface:
```
export interface CodeActionContext {
	/**
	 * An array of diagnostics known on the client side overlapping the range
	 * provided to the `textDocument/codeAction` request.
	 */
	diagnostics: Diagnostic[];

	/**
	 * Requested kind of actions to return.
	 */
	only?: CodeActionKind[];

	/**
	 * The reason why code actions were requested.
	 *
	 * @since 3.17.0
	 */
	triggerKind?: CodeActionTriggerKind;
}
```

Interface:
```
export interface CodeAction {

	/**
	 * A short, human-readable, title for this code action.
	 */
	title: string;

	/**
	 * The kind of the code action.
	 */
	kind?: CodeActionKind;

	/**
	 * The diagnostics that this code action resolves.
	 */
	diagnostics?: Diagnostic[];

	/**
	 * Marks this as a preferred action.
	 */
	isPreferred?: boolean;

	/**
	 * Marks that the code action cannot currently be applied.
	 */
	disabled?: {
		/**
		 * Human readable description of why the code action is currently
		 * disabled.
		 */
		reason: string;
	};

	/**
	 * The workspace edit this code action performs.
	 */
	edit?: WorkspaceEdit;

	/**
	 * A command this code action executes.
	 */
	command?: Command;

	/**
	 * A data entry field that is preserved on a code action between
	 * a `textDocument/codeAction` and a `codeAction/resolve` request.
	 */
	data?: LSPAny;
}
```

## Code Action Resolve
The `codeAction/resolve` request resolves additional data for a code action.

Request:
- method: 'codeAction/resolve'
- params: CodeAction

Response:
- result: CodeAction
- error: code and message if an exception happens during the request

## Document Color
The `textDocument/documentColor` request returns color information in a document.

Request:
- method: 'textDocument/documentColor'
- params: DocumentColorParams

Response:
- result: ColorInformation[]
- error: code and message if an exception happens during the request

Interface:
```
interface ColorInformation {
	/**
	 * The range in the document where this color appears.
	 */
	range: Range;

	/**
	 * The actual color value for this color range.
	 */
	color: Color;
}
```

Interface:
```
interface Color {

	/**
	 * The red component of this color in the range [0-1].
	 */
	readonly red: decimal;

	/**
	 * The green component of this color in the range [0-1].
	 */
	readonly green: decimal;

	/**
	 * The blue component of this color in the range [0-1].
	 */
	readonly blue: decimal;

	/**
	 * The alpha component of this color in the range [0-1].
	 */
	readonly alpha: decimal;
}
```

## Color Presentation
The `textDocument/colorPresentation` request returns presentation options for a colour.

Request:
- method: 'textDocument/colorPresentation'
- params: ColorPresentationParams

Response:
- result: ColorPresentation[]
- error: code and message if an exception happens during the request

Interface:
```
interface ColorPresentation {
	/**
	 * The label of this color presentation. It will be shown on the color
	 * picker header. By default this is also the text that is inserted when
	 * selecting this color presentation.
	 */
	label: string;
	/**
	 * An [edit](#TextEdit) which is applied to a document when selecting
	 * this presentation for the color. When omitted the
	 * [label](#ColorPresentation.label) is used.
	 */
	textEdit?: TextEdit;
	/**
	 * An optional array of additional [text edits](#TextEdit) that are applied
	 * when selecting this color presentation. Edits must not overlap with the
	 * main [edit](#ColorPresentation.textEdit) nor with themselves.
	 */
	additionalTextEdits?: TextEdit[];
}
```

## Formatting
The `textDocument/formatting` request returns edits to format a whole document.

Request:
- method: 'textDocument/formatting'
- params: DocumentFormattingParams

Response:
- result: TextEdit[] | null
- error: code and message if an exception happens during the request

Interface:
```
interface DocumentFormattingParams extends WorkDoneProgressParams {
	/**
	 * The document to format.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The format options.
	 */
	options: FormattingOptions;
}
```

Interface:
```
interface FormattingOptions {
	/**
	 * Size of a tab in spaces.
	 */
	tabSize: uinteger;

	/**
	 * Prefer spaces over tabs.
	 */
	insertSpaces: boolean;

	/**
	 * Trim trailing whitespace on a line.
	 *
	 * @since 3.15.0
	 */
	trimTrailingWhitespace?: boolean;

	/**
	 * Insert a newline character at the end of the file if one does not exist.
	 *
	 * @since 3.15.0
	 */
	insertFinalNewline?: boolean;

	/**
	 * Trim all newlines after the final newline at the end of the file.
	 *
	 * @since 3.15.0
	 */
	trimFinalNewlines?: boolean;

	/**
	 * Signature for further properties.
	 */
	[key: string]: boolean | integer | string;
}
```

## Range Formatting
The `textDocument/rangeFormatting` request returns edits to format a range.

Request:
- method: 'textDocument/rangeFormatting'
- params: DocumentRangeFormattingParams

Response:
- result: TextEdit[] | null
- error: code and message if an exception happens during the request

Interface:
```
interface DocumentRangeFormattingParams extends WorkDoneProgressParams {
	/**
	 * The document to format.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The range to format
	 */
	range: Range;

	/**
	 * The format options
	 */
	options: FormattingOptions;
}
```

## On Type Formatting
The `textDocument/onTypeFormatting` request returns edits triggered by typing.

Request:
- method: 'textDocument/onTypeFormatting'
- params: DocumentOnTypeFormattingParams

Response:
- result: TextEdit[] | null
- error: code and message if an exception happens during the request

Interface:
```
interface DocumentOnTypeFormattingParams {

	/**
	 * The document to format.
	 */
	textDocument: TextDocumentIdentifier;

	/**
	 * The position around which the on type formatting should happen.
	 * This is not necessarily the exact position where the character denoted
	 * by the property `ch` got typed.
	 */
	position: Position;

	/**
	 * The character that has been typed that triggered the formatting
	 * on type request. That is not necessarily the last character that
	 * got inserted into the document since the client could auto insert
	 * characters as well (e.g. like automatic brace completion).
	 */
	ch: string;

	/**
	 * The formatting options.
	 */
	options: FormattingOptions;
}
```

## Rename
The `textDocument/rename` request returns a workspace edit for renaming a symbol.

Request:
- method: 'textDocument/rename'
- params: RenameParams

Response:
- result: WorkspaceEdit | null
- error: code and message if an exception happens during the request

Interface:
```
interface RenameParams extends TextDocumentPositionParams,
	WorkDoneProgressParams {
	/**
	 * The new name of the symbol. If the given name is not valid the
	 * request must return a [ResponseError](#ResponseError) with an
	 * appropriate message set.
	 */
	newName: string;
}
```

## Prepare Rename
The `textDocument/prepareRename` request checks if a rename is valid and returns the range and placeholder.

Request:
- method: 'textDocument/prepareRename'
- params: PrepareRenameParams

Response:
- result: Range | PrepareRenameResult | null
- error: code and message if an exception happens during the request

Interface:
```
export interface PrepareRenameParams extends TextDocumentPositionParams, WorkDoneProgressParams {
}
```

## Linked Editing Range
The `textDocument/linkedEditingRange` request returns ranges that should be edited together.

Request:
- method: 'textDocument/linkedEditingRange'
- params: LinkedEditingRangeParams

Response:
- result: LinkedEditingRanges | null
- error: code and message if an exception happens during the request

Interface:
```
export interface LinkedEditingRangeParams extends TextDocumentPositionParams,
	WorkDoneProgressParams {
}
```

Interface:
```
export interface LinkedEditingRanges {
	/**
	 * A list of ranges that can be renamed together. The ranges must have
	 * identical length and contain identical text content. The ranges cannot
	 * overlap.
	 */
	ranges: Range[];

	/**
	 * An optional word pattern (regular expression) that describes valid
	 * contents for the given ranges. If no pattern is provided, the client
	 * configuration's word pattern will be used.
	 */
	wordPattern?: string;
}
```
