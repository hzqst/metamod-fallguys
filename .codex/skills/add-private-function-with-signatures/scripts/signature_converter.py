#!/usr/bin/env python3
"""
Signature Format Converter

Converts between three signature formats used in reverse engineering:
1. C_STRING:  \\x83\\xEC\\x2C\\xA1\\x2A\\x2A\\x2A\\x2A  (C/C++ string literal)
2. SPACED:    83 EC 2C A1 2A 2A 2A 2A                  (IDA Pro search format)
3. COMPACT:   83EC2CA12A2A2A2A                          (Compact hex string)

Wildcards are preserved during conversion:
- \\x2A or \\x?? in C_STRING
- ?? in SPACED
- 2A or ?? in COMPACT

Usage:
    signature_converter.py <format> <signature>

Examples:
    # Convert C string to IDA format
    signature_converter.py spaced "\\x83\\xEC\\x2C\\xA1\\x2A\\x2A\\x2A\\x2A"
    Output: 83 EC 2C A1 ?? ?? ?? ??

    # Convert IDA format to C string
    signature_converter.py c_string "83 EC 2C A1 ?? ?? ?? ??"
    Output: \\x83\\xEC\\x2C\\xA1\\x2A\\x2A\\x2A\\x2A

    # Convert compact to spaced
    signature_converter.py spaced "83EC2CA12A2A2A2A"
    Output: 83 EC 2C A1 2A 2A 2A 2A

    # Auto-detect input format and convert to all formats
    signature_converter.py all "\\x83\\xEC\\x2C"
"""

import sys
import re


class SignatureConverter:
    """Converts between different signature format representations."""

    WILDCARD_C = '2A'      # Wildcard in C string format (\\x2A)
    WILDCARD_IDA = '??'    # Wildcard in IDA Pro format

    @staticmethod
    def detect_format(signature):
        """
        Auto-detect signature format.

        Returns: 'c_string', 'spaced', 'compact', or None
        """
        signature = signature.strip()

        # Check for C string format (contains \\x)
        if '\\x' in signature:
            return 'c_string'

        # Check for spaced format (contains spaces)
        if ' ' in signature:
            return 'spaced'

        # Check for compact format (only hex chars and ??)
        if re.match(r'^[0-9A-Fa-f?]+$', signature):
            return 'compact'

        return None

    @staticmethod
    def normalize_to_bytes(signature):
        """
        Convert any format to list of byte strings.
        Wildcards are normalized to '??'.

        Returns: list of strings, e.g., ['83', 'EC', '??', '2A']
        """
        signature = signature.strip()
        fmt = SignatureConverter.detect_format(signature)

        if fmt == 'c_string':
            # Parse \\xHH format
            # Replace \\x2A and \\x?? with ??
            parts = signature.split('\\x')[1:]  # Skip empty first element
            result = []
            for part in parts:
                # Take first 2 characters
                byte = part[:2].upper()
                if byte == SignatureConverter.WILDCARD_C or byte == '??':
                    result.append('??')
                elif re.match(r'^[0-9A-Fa-f]{2}$', byte):
                    result.append(byte)
                else:
                    raise ValueError(f"Invalid byte in C string format: {byte}")
            return result

        elif fmt == 'spaced':
            # Parse space-separated format
            parts = signature.split()
            result = []
            for byte in parts:
                byte = byte.upper()
                if byte == '??':
                    result.append('??')
                elif re.match(r'^[0-9A-Fa-f]{2}$', byte):
                    result.append(byte)
                else:
                    raise ValueError(f"Invalid byte in spaced format: {byte}")
            return result

        elif fmt == 'compact':
            # Parse compact format (no spaces)
            result = []
            i = 0
            while i < len(signature):
                # Check for ?? wildcard
                if i + 1 < len(signature) and signature[i:i+2] == '??':
                    result.append('??')
                    i += 2
                # Check for regular byte
                elif i + 1 < len(signature):
                    byte = signature[i:i+2].upper()
                    if re.match(r'^[0-9A-Fa-f]{2}$', byte):
                        result.append(byte)
                        i += 2
                    else:
                        raise ValueError(f"Invalid byte in compact format: {byte}")
                else:
                    raise ValueError("Odd number of characters in compact format")
            return result

        else:
            raise ValueError("Unable to detect signature format")

    @staticmethod
    def to_c_string(signature, wildcard='2A'):
        """
        Convert to C string format: \\x83\\xEC\\x2A

        Args:
            signature: Input signature in any format
            wildcard: How to represent wildcards ('2A' or '??')
        """
        bytes_list = SignatureConverter.normalize_to_bytes(signature)

        result = []
        for byte in bytes_list:
            if byte == '??':
                if wildcard == '??':
                    result.append('\\x??')
                else:
                    result.append('\\x2A')
            else:
                result.append(f'\\x{byte}')

        return ''.join(result)

    @staticmethod
    def to_spaced(signature):
        """
        Convert to spaced format: 83 EC 2C A1 ?? ?? ?? ??
        """
        bytes_list = SignatureConverter.normalize_to_bytes(signature)
        return ' '.join(bytes_list)

    @staticmethod
    def to_compact(signature, wildcard='2A'):
        """
        Convert to compact format: 83EC2CA12A2A2A2A

        Args:
            signature: Input signature in any format
            wildcard: How to represent wildcards ('2A' or '??')
        """
        bytes_list = SignatureConverter.normalize_to_bytes(signature)

        result = []
        for byte in bytes_list:
            if byte == '??':
                result.append(wildcard)
            else:
                result.append(byte)

        return ''.join(result)

    @staticmethod
    def convert_all(signature):
        """
        Convert to all formats and return a dict.
        """
        detected = SignatureConverter.detect_format(signature)

        return {
            'detected_format': detected,
            'c_string_2a': SignatureConverter.to_c_string(signature, wildcard='2A'),
            'c_string_qq': SignatureConverter.to_c_string(signature, wildcard='??'),
            'spaced': SignatureConverter.to_spaced(signature),
            'compact_2a': SignatureConverter.to_compact(signature, wildcard='2A'),
            'compact_qq': SignatureConverter.to_compact(signature, wildcard='??'),
        }


def print_usage():
    """Print usage information."""
    print(__doc__)


def main():
    """Command-line interface."""
    if len(sys.argv) < 2:
        print_usage()
        sys.exit(1)

    if sys.argv[1] in ['-h', '--help', 'help']:
        print_usage()
        sys.exit(0)

    if len(sys.argv) < 3:
        print("Error: Missing signature argument")
        print_usage()
        sys.exit(1)

    target_format = sys.argv[1].lower()
    signature = sys.argv[2]

    try:
        if target_format == 'all':
            # Convert to all formats
            results = SignatureConverter.convert_all(signature)
            print(f"Detected format: {results['detected_format']}")
            print()
            print("C String (\\x2A wildcard):")
            print(f"  {results['c_string_2a']}")
            print()
            print("C String (?? wildcard):")
            print(f"  {results['c_string_qq']}")
            print()
            print("Spaced (IDA Pro):")
            print(f"  {results['spaced']}")
            print()
            print("Compact (2A wildcard):")
            print(f"  {results['compact_2a']}")
            print()
            print("Compact (?? wildcard):")
            print(f"  {results['compact_qq']}")

        elif target_format in ['c', 'c_string', 'cstring']:
            # Convert to C string
            wildcard = '2A'
            if len(sys.argv) > 3 and sys.argv[3] in ['??', 'qq']:
                wildcard = '??'
            result = SignatureConverter.to_c_string(signature, wildcard=wildcard)
            print(result)

        elif target_format in ['s', 'spaced', 'ida']:
            # Convert to spaced
            result = SignatureConverter.to_spaced(signature)
            print(result)

        elif target_format in ['compact', 'c']:
            # Convert to compact
            wildcard = '2A'
            if len(sys.argv) > 3 and sys.argv[3] in ['??', 'qq']:
                wildcard = '??'
            result = SignatureConverter.to_compact(signature, wildcard=wildcard)
            print(result)

        else:
            print(f"Error: Unknown format '{target_format}'")
            print("Valid formats: c_string, spaced, compact, all")
            sys.exit(1)

    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()
