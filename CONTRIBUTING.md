# Contributing to ImageEncryption

Thank you for your interest in contributing to ImageEncryption! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

Be respectful and constructive in all interactions with other contributors.

## How to Contribute

### Reporting Bugs

If you find a bug, please create an issue with:
- Clear description of the bug
- Steps to reproduce
- Expected behavior
- Actual behavior
- Your environment (OS, Python version, etc.)

### Suggesting Features

Feature suggestions are welcome! Please create an issue describing:
- The feature you'd like to see
- Use cases for this feature
- Possible implementation approach (optional)

### Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Make your changes
4. Add or update tests as needed
5. Ensure all tests pass
6. Commit your changes (`git commit -m 'Add AmazingFeature'`)
7. Push to your branch (`git push origin feature/AmazingFeature`)
8. Open a Pull Request

## Development Setup

1. Clone the repository
2. Create a virtual environment:
   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```
3. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```
4. Run tests:
   ```bash
   python -m pytest tests/
   ```

## Coding Standards

- Follow PEP 8 style guide
- Use meaningful variable and function names
- Add docstrings to functions and classes
- Write descriptive commit messages
- Add tests for new features

## Testing

- Write unit tests for new features
- Ensure all tests pass before submitting PR
- Aim for good test coverage

## Documentation

- Update README.md if needed
- Add docstrings to new code
- Update CHANGELOG.md with notable changes

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Questions?

Feel free to create an issue with your questions!
