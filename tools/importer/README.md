# Catalogue importer

- `reference.py` contains the explicit source inventory, the frozen TH18
  reference, and supplements whose hashes are verified.
- `../import_data.py` normalizes raw documents and writes the catalogue and its
  manifest.

A new entry therefore starts in `reference.py`. Its transformation remains in
`import_data.py`. Combat rules never belong in these tools.
