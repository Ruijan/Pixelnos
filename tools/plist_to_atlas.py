#!/usr/bin/env python3
"""
Convert Cocos2d .plist sprite sheet definitions to Bevy-compatible JSON atlas layouts.

Reads TexturePacker-format .plist files and outputs JSON files that can be loaded
by Bevy's TextureAtlasLayout system.

Usage:
    python3 plist_to_atlas.py <input.plist> [output.json]
    python3 plist_to_atlas.py --all <assets_dir>

Output format (per frame):
    {
        "texture": "cut.png",
        "size": [1090, 453],
        "frames": [
            {"name": "cut_000.png", "x": 0, "y": 0, "w": 218, "h": 151},
            ...
        ]
    }
"""

import json
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


def parse_cocos2d_rect(s: str) -> tuple[int, int, int, int]:
    """Parse '{{x,y},{w,h}}' format."""
    nums = [int(n) for n in re.findall(r'-?\d+', s)]
    return nums[0], nums[1], nums[2], nums[3]


def parse_cocos2d_size(s: str) -> tuple[int, int]:
    """Parse '{w,h}' format."""
    nums = [int(n) for n in re.findall(r'-?\d+', s)]
    return nums[0], nums[1]


def parse_plist_dict(element) -> dict:
    """Parse a plist <dict> element into a Python dict."""
    result = {}
    children = list(element)
    i = 0
    while i < len(children):
        if children[i].tag == 'key':
            key = children[i].text
            i += 1
            value_elem = children[i]
            if value_elem.tag == 'dict':
                result[key] = parse_plist_dict(value_elem)
            elif value_elem.tag == 'string':
                result[key] = value_elem.text or ''
            elif value_elem.tag == 'integer':
                result[key] = int(value_elem.text)
            elif value_elem.tag == 'real':
                result[key] = float(value_elem.text)
            elif value_elem.tag == 'true':
                result[key] = True
            elif value_elem.tag == 'false':
                result[key] = False
            elif value_elem.tag == 'array':
                result[key] = [c.text for c in value_elem if c.text]
            else:
                result[key] = None
        i += 1
    return result


def convert_plist_to_atlas(plist_path: str) -> dict:
    """Convert a Cocos2d plist sprite sheet to Bevy atlas JSON."""
    tree = ET.parse(plist_path)
    root = tree.getroot()

    # The root <dict> contains 'frames' and 'metadata'
    root_dict = parse_plist_dict(root.find('dict'))
    frames_dict = root_dict.get('frames', {})
    metadata = root_dict.get('metadata', {})

    texture_file = metadata.get('textureFileName', metadata.get('realTextureFileName', ''))
    texture_size = parse_cocos2d_size(metadata.get('size', '{0,0}'))

    frames = []
    for name, frame_data in sorted(frames_dict.items()):
        rect = frame_data.get('textureRect', frame_data.get('frame', '{{0,0},{0,0}}'))
        x, y, w, h = parse_cocos2d_rect(rect)

        rotated = frame_data.get('textureRotated', False)

        frames.append({
            'name': name,
            'x': x,
            'y': y,
            'w': w,
            'h': h,
            'rotated': rotated,
        })

    return {
        'texture': texture_file,
        'size': list(texture_size),
        'frames': frames,
    }


def convert_file(plist_path: str, output_path: str | None = None):
    """Convert a single plist file."""
    atlas = convert_plist_to_atlas(plist_path)
    if output_path is None:
        output_path = str(Path(plist_path).with_suffix('.atlas.json'))

    with open(output_path, 'w') as f:
        json.dump(atlas, f, indent=2)

    print(f"  {plist_path} -> {output_path} ({len(atlas['frames'])} frames)")
    return output_path


def convert_all(assets_dir: str):
    """Find and convert all .plist files under the assets directory."""
    assets_path = Path(assets_dir)
    plist_files = sorted(assets_path.rglob('*.plist'))

    if not plist_files:
        print(f"No .plist files found in {assets_dir}")
        return

    print(f"Converting {len(plist_files)} plist files...")
    for plist_file in plist_files:
        convert_file(str(plist_file))
    print("Done!")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    if sys.argv[1] == '--all':
        if len(sys.argv) < 3:
            print("Usage: plist_to_atlas.py --all <assets_dir>")
            sys.exit(1)
        convert_all(sys.argv[2])
    else:
        output = sys.argv[2] if len(sys.argv) > 2 else None
        convert_file(sys.argv[1], output)
