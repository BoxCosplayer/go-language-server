Development is explicitly to support GOatpad, however values and functions shouldnt be hardcoded to strictly match GOatpad functionality.
In other words, though this project should be made to support Goatpad first (which should definelty be kept in mind during development and should dictate the order of features being built), the LSP specification needs to be adhered to. 

See ./notes/lsp-notes.md for more details.
note: all notes are updates for lsp version 3.17, and does not include depreceated features.

Requirements:

- The server should be interfacable using JSON-RPC.
- Expected i/o is defined in lsp-notes.md
- Capabilties/features defined in capabilities.md, should be listed once lsp-notes is near-completion.

Lifecycle of the server (updated with my understanding):
- Client spawns an instance of the server
- Client send an `Initialise` request (all other requests/notifications are dropped, par exits)
- Server sends `InitialiseResult` response
- Client-Server negotiates different capabilities to determine appropriate server-side logic
- Client sends requests, which are linearly processed by the server
- Server completes actions, with logic based on capabilities and returns a response
- Client can query progress for some tasks before a response is sent
- Client continues to send requests
- Client shutsdown process with an `exit notification`

Current TODOS {
    utils: [decoding JSON, decoding URI format],
}
