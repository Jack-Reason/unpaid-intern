# unpaid-intern brain

Provider-agnostic LLM brain and localhost bridge client.

## Defaults

- **Local:** Ollama at `http://127.0.0.1:11434` (model `llama3.2` unless `OLLAMA_MODEL` set)
- **Cloud:** set `OPENAI_API_KEY` (+ optional `OPENAI_BASE_URL`, `OPENAI_MODEL`) for any OpenAI-compatible API

## Quickstart (Windows PowerShell)

```powershell
cd brain
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -e ".[dev]"
pytest
python -m unpaid_intern.cli mock   # mock game + one dry-run tick
```

Or from repo root: `..\scripts\Setup-Brain.ps1` then `..\scripts\Run-Brain.ps1`.
