## Development Setup

__Create and activate a virtual environment__
```bash
python3 -m venv venv
source venv/bin/activate  # Linux/Mac
venv\Scripts\activate     # Windows
```

__install/update package__
```bash
cd <package>/sfix
pip install -e .
```

__run tests suits__
```bash
cd <package>/sfix
python3 -m unittest discover
```


