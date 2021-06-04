open Bimage
open Graphics

(** Converts a Graphics.color [c] into its rgb code. *)
let color_to_rgb c =
  let r = c / 65536
  and g = c / 256 mod 256
  and b = c mod 256 in
  r, g, b
;;

(** Colors the pixel at ([x], [y]) of [img] with the color [c] *)
let color_pixel img x y c =
  let r, g, b = color_to_rgb c in
  Image.set img x y 0 r;
  Image.set img x y 1 g;
  Image.set img x y 2 b
;;

(** Save the drawing as a PNG in [path] *)
let save path =
  let color_matrix = dump_image (get_image 0 0 Cli.dimx Cli.dimy) in
  let img = Image.create u8 Bimage.rgb Cli.dimx Cli.dimy in
  let _ = Image.for_each (fun x y _px -> color_pixel img x y color_matrix.(y).(x)) img in
  Bimage_unix.Magick.write path img
;;
