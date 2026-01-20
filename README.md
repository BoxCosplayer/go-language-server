Development is explicitly to support GOatpad, however values and functions shouldnt be hardcoded to strictly match GOatpad functionality.
In other words, though this project should be made to support Goatpad first (which should definelty be kept in mind during development and should dictate the order of features being built), the LSP specification needs to be adhered to. 

See lsp-notes.md for more details.

Requirements:

The server should be interfacable using JSON-RPC.
Expected i/o is defined in lsp-notes.md
Capabilties/features defined in capabilities.md, should be listed once lsp-notes is near-completion.

Current TODOS {
    utils: [decoding JSON, decoding URI format],
}
