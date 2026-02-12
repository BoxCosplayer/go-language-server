# Misc Notes

## DocumentUri
`DocumentUri` is the LSP type used to identify a text document across requests,
responses, and notifications.

Type:
```ts
export type URI = string;
export type DocumentUri = string;
```

Notes:
- A `DocumentUri` is a URI string, not a filesystem path.
- It should follow RFC 3986 URI rules.
- Most document URIs are absolute.
- The same open document should keep the same URI identity through its lifecycle.

Common schemes:
- `file` (regular files)
- `untitled` (unsaved buffers)
- editor specific schemes (for virtual documents)

Interface examples:
```ts
export interface TextDocumentIdentifier {
	uri: DocumentUri;
}

export interface TextDocumentItem {
	uri: DocumentUri;
	languageId: string;
	version: integer;
	text: string;
}

export interface OptionalVersionedTextDocumentIdentifier extends TextDocumentIdentifier {
	version: integer | null;
}
```

## Practical Handling Notes
- Parse URIs with a URI parser; do not hand-build them with string concatenation.
- Do not assume `file` scheme; features should tolerate non-file schemes.
- Keep URI strings protocol-safe (percent encoding matters).
- For internal lookup maps, normalize consistently, especially on Windows `file` URIs.
- When sending values back over LSP, keep protocol form as URI strings.

## Common Pitfalls
- Treating a `DocumentUri` as a plain OS path.
- Comparing raw URI strings without normalization rules.
- Dropping query/fragment parts that may be meaningful in virtual documents.
