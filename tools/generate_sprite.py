#!/usr/bin/env python3
"""
CLI tool for generating MallowsVsDango game sprites using OpenAI's image API.

Usage:
    ./generate_sprite.py tower "ice marshmallow that freezes enemies" --size 200x226
    ./generate_sprite.py dango "spicy dango with flames" --size 231x190
    ./generate_sprite.py background "volcanic candy land" --size 779x660
    ./generate_sprite.py object "candy cane tree" --size 324x200
    ./generate_sprite.py ui "round wooden button with sugar icon" --size 120x120
    ./generate_sprite.py effect "water splash impact" --size 100x100

    # Generate 4 variants to pick from
    ./generate_sprite.py tower "ninja marshmallow" --variants 4

    # Use an existing sprite as style reference
    ./generate_sprite.py tower "healer marshmallow" --reference res/turret/general.png

    # Override the art style
    ./generate_sprite.py tower "dark marshmallow" --style-override "dark gothic cartoon style"

    # List available asset types and their defaults
    ./generate_sprite.py --list-types

Requires:
    pip install openai Pillow

Environment:
    OPENAI_API_KEY must be set.
"""

import argparse
import base64
import io
import os
import sys
import json
import time
from pathlib import Path

try:
    from openai import OpenAI
except ImportError:
    print("Error: openai package not installed. Run: pip install openai", file=sys.stderr)
    sys.exit(1)

try:
    from PIL import Image
except ImportError:
    print("Error: Pillow package not installed. Run: pip install Pillow", file=sys.stderr)
    sys.exit(1)


# ---------------------------------------------------------------------------
# Art style definitions
# ---------------------------------------------------------------------------

BASE_STYLE = (
    "2D game sprite with transparent background. "
    "Cute chibi cartoon style with bold black outlines (2-3px weight). "
    "Flat pastel colors with minimal shading — at most a single soft gradient per shape. "
    "Round, blob-like proportions. Simple dot eyes and small expressive mouths. "
    "Clean vector-art look, no texture noise, no sketchy lines. "
    "The character should be centered in the frame with generous transparent padding around it. "
    "Style reference: similar to Kirby, Cut the Rope, or Slime Rancher character designs."
)

ASSET_TYPES = {
    "tower": {
        "default_size": (200, 226),
        "prompt_prefix": (
            "A marshmallow-themed tower defense tower character. "
            "The character is a cute marshmallow (soft white/pink rounded rectangular body). "
            "It has a distinct personality shown through accessories (hats, weapons, expressions). "
            "Candy and sweets themed equipment. "
        ),
        "prompt_suffix": (
            "The character should look like it belongs in a candy/sweets themed tower defense game. "
            "Facing slightly to the right. Full body visible."
        ),
        "api_size": "1024x1024",
    },
    "dango": {
        "default_size": (231, 190),
        "prompt_prefix": (
            "A dango-themed enemy creature for a tower defense game. "
            "The creature is a cute round/oval blob shape (like a Japanese dango or mochi ball). "
            "Soft pink or pastel colored body with a shiny highlight on top. "
            "Simple face with dot eyes. "
        ),
        "prompt_suffix": (
            "The creature should look like a cute but mischievous enemy in a candy-themed game. "
            "Slightly bouncy pose as if moving to the right."
        ),
        "api_size": "1024x1024",
    },
    "background": {
        "default_size": (779, 660),
        "prompt_prefix": (
            "A 2D game level background for a candy/sweets themed tower defense game. "
            "Colorful, cheerful environment with soft gradients. "
            "No characters or UI elements — just the environment. "
        ),
        "prompt_suffix": (
            "The background should tile-friendly edges where possible. "
            "Vibrant but not overwhelming colors. Top-down slight angle perspective."
        ),
        "api_size": "1792x1024",
    },
    "object": {
        "default_size": (200, 150),
        "prompt_prefix": (
            "A decorative game object sprite with transparent background. "
            "Candy/sweets themed environment decoration for a 2D tower defense game. "
            "Bold black outlines, flat colors, cartoon style. "
        ),
        "prompt_suffix": (
            "Clean isolated object, no ground plane, transparent background."
        ),
        "api_size": "1024x1024",
    },
    "ui": {
        "default_size": (120, 120),
        "prompt_prefix": (
            "A UI element for a cute candy-themed mobile/PC game. "
            "Bold outlines, flat colors, rounded corners. "
            "Clean and readable at small sizes. "
        ),
        "prompt_suffix": (
            "Isolated on transparent background. Simple and iconic."
        ),
        "api_size": "1024x1024",
    },
    "effect": {
        "default_size": (100, 100),
        "prompt_prefix": (
            "A 2D game visual effect sprite with transparent background. "
            "Cartoon style with bold outlines. "
            "Candy/sweets themed (chocolate splashes, sugar sparkles, etc). "
        ),
        "prompt_suffix": (
            "Isolated effect, transparent background, suitable for overlay."
        ),
        "api_size": "1024x1024",
    },
}


def build_prompt(asset_type: str, description: str, style_override: str | None = None) -> str:
    """Build the full generation prompt from templates + user description."""
    type_info = ASSET_TYPES[asset_type]
    style = style_override if style_override else BASE_STYLE

    prompt = (
        f"{type_info['prompt_prefix']}"
        f"{description}. "
        f"{type_info['prompt_suffix']} "
        f"{style}"
    )
    return prompt


def encode_reference_image(path: str) -> str:
    """Load and encode a reference image to base64 for the API."""
    with open(path, "rb") as f:
        return base64.b64encode(f.read()).decode("utf-8")


def generate_image(
    client: OpenAI,
    prompt: str,
    api_size: str,
    reference_path: str | None = None,
    quality: str = "high",
) -> bytes:
    """Call OpenAI image generation API and return raw PNG bytes."""
    if reference_path:
        # Use the image edit endpoint with reference
        with open(reference_path, "rb") as ref_file:
            result = client.images.edit(
                model="gpt-image-1",
                image=ref_file,
                prompt=prompt,
                size=api_size,
            )
    else:
        result = client.images.generate(
            model="gpt-image-1",
            prompt=prompt,
            size=api_size,
            quality=quality,
            n=1,
        )

    image_b64 = result.data[0].b64_json
    return base64.b64decode(image_b64)


def postprocess(raw_bytes: bytes, target_w: int, target_h: int) -> Image.Image:
    """Resize generated image to exact target dimensions, preserving transparency."""
    img = Image.open(io.BytesIO(raw_bytes)).convert("RGBA")

    # Find the bounding box of non-transparent content
    bbox = img.getbbox()
    if bbox:
        img = img.crop(bbox)

    # Resize to fit within target dimensions, maintaining aspect ratio
    img.thumbnail((target_w, target_h), Image.LANCZOS)

    # Center on a transparent canvas of exact target size
    canvas = Image.new("RGBA", (target_w, target_h), (0, 0, 0, 0))
    offset_x = (target_w - img.width) // 2
    offset_y = (target_h - img.height) // 2
    canvas.paste(img, (offset_x, offset_y), img)

    return canvas


def generate_output_path(output_dir: str, asset_type: str, description: str, index: int = 0) -> Path:
    """Generate a descriptive output filename."""
    # Create a slug from the description
    slug = description.lower()[:40]
    slug = "".join(c if c.isalnum() or c == " " else "" for c in slug)
    slug = slug.strip().replace(" ", "_")
    if not slug:
        slug = "sprite"

    suffix = f"_{index}" if index > 0 else ""
    filename = f"{asset_type}_{slug}{suffix}.png"
    return Path(output_dir) / filename


def parse_size(size_str: str) -> tuple[int, int]:
    """Parse a WxH size string."""
    parts = size_str.lower().split("x")
    if len(parts) != 2:
        raise ValueError(f"Invalid size format: {size_str}. Use WxH (e.g., 200x226)")
    return int(parts[0]), int(parts[1])


def list_types():
    """Print available asset types and their defaults."""
    print("\nAvailable asset types:\n")
    for name, info in ASSET_TYPES.items():
        w, h = info["default_size"]
        print(f"  {name:12s}  default size: {w}x{h}")
        # Print first 80 chars of prefix as description
        desc = info["prompt_prefix"][:100].strip()
        print(f"               {desc}...")
        print()


def main():
    parser = argparse.ArgumentParser(
        description="Generate MallowsVsDango game sprites via OpenAI API",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument(
        "asset_type",
        nargs="?",
        choices=list(ASSET_TYPES.keys()),
        help="Type of asset to generate",
    )
    parser.add_argument(
        "description",
        nargs="?",
        help="What to generate (e.g., 'ice marshmallow that freezes enemies')",
    )
    parser.add_argument(
        "--size",
        type=str,
        default=None,
        help="Output size as WxH (e.g., 200x226). Defaults per asset type.",
    )
    parser.add_argument(
        "--variants",
        type=int,
        default=1,
        help="Number of variants to generate (default: 1)",
    )
    parser.add_argument(
        "--reference",
        type=str,
        default=None,
        help="Path to reference image for style matching",
    )
    parser.add_argument(
        "--style-override",
        type=str,
        default=None,
        help="Override the base art style description",
    )
    parser.add_argument(
        "--output-dir",
        type=str,
        default="./generated",
        help="Output directory (default: ./generated)",
    )
    parser.add_argument(
        "--quality",
        choices=["low", "medium", "high"],
        default="high",
        help="Generation quality (default: high)",
    )
    parser.add_argument(
        "--show-prompt",
        action="store_true",
        help="Print the full prompt without generating",
    )
    parser.add_argument(
        "--list-types",
        action="store_true",
        help="List available asset types and exit",
    )

    args = parser.parse_args()

    if args.list_types:
        list_types()
        return

    if not args.asset_type or not args.description:
        parser.error("asset_type and description are required (use --list-types to see options)")

    # Build prompt
    prompt = build_prompt(args.asset_type, args.description, args.style_override)
    type_info = ASSET_TYPES[args.asset_type]

    if args.show_prompt:
        print("Full prompt:")
        print("-" * 60)
        print(prompt)
        print("-" * 60)
        return

    # Determine output size
    if args.size:
        target_w, target_h = parse_size(args.size)
    else:
        target_w, target_h = type_info["default_size"]

    # Check API key
    api_key = os.environ.get("OPENAI_API_KEY")
    if not api_key:
        print("Error: OPENAI_API_KEY environment variable not set.", file=sys.stderr)
        print("  export OPENAI_API_KEY='sk-...'", file=sys.stderr)
        sys.exit(1)

    client = OpenAI(api_key=api_key)

    # Create output directory
    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    api_size = type_info["api_size"]

    print(f"Generating {args.variants} {args.asset_type} sprite(s)...")
    print(f"  Concept: {args.description}")
    print(f"  API size: {api_size} -> resize to {target_w}x{target_h}")
    if args.reference:
        print(f"  Style reference: {args.reference}")
    print()

    generated_files = []

    for i in range(args.variants):
        variant_label = f" (variant {i + 1}/{args.variants})" if args.variants > 1 else ""
        print(f"  Generating{variant_label}...", end="", flush=True)
        start = time.time()

        try:
            raw_bytes = generate_image(
                client=client,
                prompt=prompt,
                api_size=api_size,
                reference_path=args.reference,
                quality=args.quality,
            )

            # Post-process: crop, resize, center on transparent canvas
            final_image = postprocess(raw_bytes, target_w, target_h)

            # Save
            output_path = generate_output_path(args.output_dir, args.asset_type, args.description, i)
            final_image.save(str(output_path), "PNG")

            elapsed = time.time() - start
            print(f" done ({elapsed:.1f}s) -> {output_path}")
            generated_files.append(str(output_path))

        except Exception as e:
            print(f" FAILED: {e}", file=sys.stderr)

    if generated_files:
        print(f"\nGenerated {len(generated_files)} file(s) in {args.output_dir}/")

        # Write a manifest with metadata for reproducibility
        manifest_path = output_dir / "manifest.jsonl"
        with open(manifest_path, "a") as f:
            for file_path in generated_files:
                entry = {
                    "file": file_path,
                    "asset_type": args.asset_type,
                    "description": args.description,
                    "size": f"{target_w}x{target_h}",
                    "prompt": prompt,
                    "reference": args.reference,
                    "quality": args.quality,
                    "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
                }
                f.write(json.dumps(entry) + "\n")
        print(f"Manifest updated: {manifest_path}")


if __name__ == "__main__":
    main()
