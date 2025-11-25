#include "MIPS32.h"
#include "microui.h"
#include "types.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_W 8
#define CHAR_H 8

static const unsigned char font_8x8[256][8] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00 },
  { 0x66, 0x66, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00 },
  { 0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00 },
  { 0x00, 0x66, 0x66, 0x0C, 0x18, 0x30, 0x66, 0x66 },
  { 0x3C, 0x66, 0x3C, 0x38, 0x67, 0x66, 0x3F, 0x00 },
  { 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00 },
  { 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00 },
  { 0x00, 0x18, 0x7E, 0x3C, 0x7E, 0x18, 0x00, 0x00 },
  { 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30 },
  { 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00 },
  { 0x06, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x60, 0x00 },
  { 0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00 },
  { 0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00 },
  { 0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00 },
  { 0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00 },
  { 0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00 },
  { 0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00 },
  { 0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00 },
  { 0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00 },
  { 0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00 },
  { 0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C, 0x00 },
  { 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00 },
  { 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30, 0x00 },
  { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00 },
  { 0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00 },
  { 0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00 },
  { 0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00 },
  { 0x3C, 0x66, 0x6E, 0x6E, 0x6C, 0x60, 0x3E, 0x00 },
  { 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00 },
  { 0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00 },
  { 0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00 },
  { 0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00 },
  { 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00 },
  { 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00 },
  { 0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00 },
  { 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00 },
  { 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00 },
  { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00 },
  { 0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00 },
  { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00 },
  { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00 },
  { 0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00 },
  { 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00 },
  { 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00 },
  { 0x3C, 0x66, 0x66, 0x66, 0x6E, 0x7C, 0x06, 0x00 },
  { 0x7C, 0x66, 0x66, 0x7C, 0x78, 0x6C, 0x66, 0x00 },
  { 0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00 },
  { 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 },
  { 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00 },
  { 0x66, 0x66, 0x66, 0x66, 0x36, 0x1C, 0x08, 0x00 },
  { 0x63, 0x63, 0x6B, 0x7F, 0x7F, 0x77, 0x63, 0x00 },
  { 0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00 },
  { 0x66, 0x66, 0x36, 0x1C, 0x08, 0x08, 0x08, 0x00 },
  { 0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00 },
  { 0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00 },
  { 0x60, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x06, 0x00 },
  { 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00 },
  { 0x08, 0x1C, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00 },
  { 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00 },
  { 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00 },
  { 0x00, 0x00, 0x3C, 0x60, 0x60, 0x60, 0x3C, 0x00 },
  { 0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x00 },
  { 0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00 },
  { 0x1C, 0x36, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x00 },
  { 0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x7C },
  { 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00 },
  { 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00 },
  { 0x06, 0x00, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3C },
  { 0x60, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x00 },
  { 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00 },
  { 0x00, 0x00, 0x66, 0x7F, 0x7F, 0x6B, 0x63, 0x00 },
  { 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00 },
  { 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00 },
  { 0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60 },
  { 0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06 },
  { 0x00, 0x00, 0x7C, 0x66, 0x60, 0x60, 0x60, 0x00 },
  { 0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00 },
  { 0x30, 0x30, 0x7C, 0x30, 0x30, 0x36, 0x1C, 0x00 },
  { 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00 },
  { 0x00, 0x00, 0x66, 0x66, 0x36, 0x1C, 0x08, 0x00 },
  { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x77, 0x00 },
  { 0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00 },
  { 0x00, 0x00, 0x66, 0x66, 0x3E, 0x06, 0x7C, 0x00 },
  { 0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00 },
  { 0x0E, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0E, 0x00 },
  { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 },
  { 0x70, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x70, 0x00 },
  { 0x3B, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

typedef enum {
  INPUT_MODE_HEX = 0,
  INPUT_MODE_BIN = 1,
  INPUT_MODE_DEC = 2,
  INPUT_MODE_OCT = 3
} input_mode_t;

typedef enum
{
  WRITE_TYPE_VALUE = 0,
  WRITE_TYPE_INSTRUCTION = 1
} write_type_t;

typedef enum
{
  INSTR_TYPE_R = 0,
  INSTR_TYPE_I = 1,
  INSTR_TYPE_J = 2
} instr_type_t;

typedef struct
{
  write_type_t write_type;
  instr_type_t instr_type;
  input_mode_t input_mode;
  char addr_input[32];
  char value_input[32];
  char r_opcode[16], r_rs[16], r_rt[16], r_rd[16], r_shamt[16], r_func[16];
  char i_opcode[16], i_rs[16], i_rt[16], i_imm[16];
  char j_opcode[16], j_target[32];
  char last_status[256];
  int mem_scroll_y;
  int edit_addr;
  char edit_byte[3];
  int editing;
} ui_state_t;

static void write_u32_be(u8 *memory, u32 addr, u32 value) {
  /*memory[addr + 0] = (value >> 24) & 0xFF;
  memory[addr + 1] = (value >> 16) & 0xFF;
  memory[addr + 2] = (value >> 8) & 0xFF;
  memory[addr + 3] = (value >> 0) & 0xFF;*/
  *((u32 *) (memory + addr)) = value;
}

static u32 read_u32_be(u8 *memory, u32 addr) {
  return ((u32)memory[addr + 0] << 24) |
         ((u32)memory[addr + 1] << 16) |
         ((u32)memory[addr + 2] << 8) |
         ((u32)memory[addr + 3] << 0);
}

static void val_to_str(unsigned int val, char *buf, int mode, int bits) {
  switch (mode) {
    case INPUT_MODE_HEX:
      sprintf(buf, "%X", val);
      break;
    case INPUT_MODE_BIN: {
        if (bits > 0) {
            for (int i = 0; i < bits; i++) {
                buf[i] = ((val >> (bits - 1 - i)) & 1) ? '1' : '0';
              }
            buf[bits] = '\0';
          } else
            ;

      } break;
    case INPUT_MODE_DEC:
      sprintf(buf, "%u", val);
      break;
    case INPUT_MODE_OCT:
      sprintf(buf, "%o", val);
      break;
    }
}

static unsigned int str_to_val(const char *buf, int mode, char **endptr) {
  if (!buf || !*buf) {
      if (endptr) *endptr = (char *)buf;
      return 0;
    }

  if (mode == INPUT_MODE_BIN) {
      unsigned int res = 0;
      const char *p = buf;
      while (*p == ' ') p++;

      while (*p == '0' || *p == '1') {
          res = (res << 1) | (*p - '0');
          p++;
        }

      if (endptr) *endptr = (char *)p;
      return res;
    }

  int base = 10;
  switch (mode) {
    case INPUT_MODE_HEX: base = 16; break;
    case INPUT_MODE_DEC: base = 10; break;
    case INPUT_MODE_OCT: base = 8; break;
    }
  return (unsigned int)strtoul(buf, endptr, base);
}

static void convert_fields(ui_state_t *ui, int old_mode, int new_mode) {
#define CONV(field, bits) \
      do { \
        char *end; \
        unsigned int v = str_to_val(ui->field, old_mode, &end); \
        if (ui->field[0] != '\0') val_to_str(v, ui->field, new_mode, (new_mode == INPUT_MODE_BIN) ? bits : 0); \
      } while(0)

  // R-Type
  CONV(r_opcode, 6);
  CONV(r_rs, 5);
  CONV(r_rt, 5);
  CONV(r_rd, 5);
  CONV(r_shamt, 5);
  CONV(r_func, 6);

  // I-Type
  CONV(i_opcode, 6);
  CONV(i_rs, 5);
  CONV(i_rt, 5);
  CONV(i_imm, 16);

  // J-Type
  CONV(j_opcode, 6);
  CONV(j_target, 26);

#undef CONV
}


static void ui_field(mu_Context *ctx, const char *label, char *buf, int buf_size, int mode, int bits) {
  int col_widths[2];
  col_widths[0] = 60;
  col_widths[1] = -1;
  mu_layout_row(ctx, 2, col_widths, 24);

  mu_label(ctx, label);

  char old_buf[64];
  strncpy(old_buf, buf, 64);
  old_buf[63] = '\0';

  if (mu_textbox_ex(ctx, buf, buf_size, 0) & MU_RES_CHANGE) {
      if (strlen(buf) == 0) return;

      char *end;
      unsigned int val = str_to_val(buf, mode, &end);

      unsigned int max_val = (bits >= 32) ? 0xFFFFFFFF : (1U << bits) - 1;

      // Валидация: если есть мусор (*end != 0) или число слишком большое -> откат
      if (*end != '\0' || val > max_val) {
          strcpy(buf, old_buf);
        }
    }
}

static void
render_char (SDL_Renderer *r, int x, int y, unsigned char ch, SDL_Color color)
{
  SDL_SetRenderDrawColor (r, color.r, color.g, color.b, color.a);
  const unsigned char *bitmap = font_8x8[(int)ch];
  for (int row = 0; row < 8; row++)
    {
      unsigned char byte = bitmap[row];
      for (int col = 0; col < 8; col++)
        {
          if (byte & (1 << (7 - col)))
            {
              SDL_RenderPoint (r, x + col, y + row);
            }
        }
    }
}

static void
render_text (SDL_Renderer *r, int x, int y, const char *text, SDL_Color color)
{
  int px = x;
  for (const char *p = text; *p; ++p)
    {
      if (*p == '\n')
        {
          y += CHAR_H + 2;
          px = x;
        }
      else
        {
          render_char (r, px, y, (unsigned char)*p, color);
          px += CHAR_W;
        }
    }
}

static int
text_width (mu_Font font, const char *text, int len)
{
  if (len == -1)
    len = (int)strlen (text);
  return len * CHAR_W;
}

static int
text_height (mu_Font font)
{
  return CHAR_H;
}

static void
set_color (SDL_Renderer *r, mu_Color c)
{
  SDL_SetRenderDrawColor (r, c.r, c.g, c.b, c.a);
}

static void
draw_icon (SDL_Renderer *r, int id, mu_Rect rect)
{
  int cx = rect.x + (rect.w - 8) / 2;
  int cy = rect.y + (rect.h - 8) / 2;
  set_color (r, mu_color (255, 255, 255, 255));
  switch (id)
    {
    case MU_ICON_CLOSE:
      SDL_RenderLine (r, cx, cy, cx + 8, cy + 8);
      SDL_RenderLine (r, cx + 8, cy, cx, cy + 8);
      break;
    case MU_ICON_CHECK:
      SDL_RenderLine (r, cx, cy + 4, cx + 4, cy + 8);
      SDL_RenderLine (r, cx + 4, cy + 8, cx + 8, cy);
      break;
    default:
      SDL_RenderFillRect (r, &(SDL_FRect){ cx + 3, cy + 3, 2, 2 });
      break;
    }
}

static void
render_microui (SDL_Renderer *renderer, mu_Context *ctx)
{
  mu_Command *cmd = NULL;
  while (mu_next_command (ctx, &cmd))
    {
      switch (cmd->type)
        {
        case MU_COMMAND_TEXT:
          {
            SDL_Color col = { cmd->text.color.r, cmd->text.color.g,
                              cmd->text.color.b, cmd->text.color.a };
            render_text (renderer, cmd->text.pos.x, cmd->text.pos.y,
                         cmd->text.str, col);
          }
          break;
        case MU_COMMAND_RECT:
          {
            set_color (renderer, cmd->rect.color);
            SDL_FRect rect
                = { (float)cmd->rect.rect.x, (float)cmd->rect.rect.y,
                    (float)cmd->rect.rect.w, (float)cmd->rect.rect.h };
            SDL_RenderFillRect (renderer, &rect);
          }
          break;
        case MU_COMMAND_ICON:
          draw_icon (renderer, cmd->icon.id, cmd->icon.rect);
          break;
        case MU_COMMAND_CLIP:
          {
            SDL_Rect clip = { cmd->clip.rect.x, cmd->clip.rect.y,
                              cmd->clip.rect.w, cmd->clip.rect.h };
            SDL_SetRenderClipRect (renderer,
                                   (clip.w > 0 && clip.h > 0) ? &clip : NULL);
          }
          break;
        }
    }
}

static void
handle_keyboard_input (mu_Context *ctx, SDL_Event *e)
{
  int is_down = (e->type == SDL_EVENT_KEY_DOWN);

  switch (e->key.key)
    {
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
      is_down ? mu_input_keydown (ctx, MU_KEY_SHIFT)
              : mu_input_keyup (ctx, MU_KEY_SHIFT);
      break;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
      is_down ? mu_input_keydown (ctx, MU_KEY_CTRL)
              : mu_input_keyup (ctx, MU_KEY_CTRL);
      break;
    case SDLK_BACKSPACE:
      is_down ? mu_input_keydown (ctx, MU_KEY_BACKSPACE)
              : mu_input_keyup (ctx, MU_KEY_BACKSPACE);
      break;
    case SDLK_RETURN:
      is_down ? mu_input_keydown (ctx, MU_KEY_RETURN)
              : mu_input_keyup (ctx, MU_KEY_RETURN);
      break;
    }
}

static void
window_registers (mu_Context *ctx, MIPS32_t *cpu, ui_state_t *ui)
{
  if (mu_begin_window_ex (ctx, "Registers", mu_rect (10, 10, 500, 400),
                          MU_OPT_NORESIZE))
    {
      char buf[64];

      mu_layout_row (ctx, 1, (int[]){ -1 }, 22);
      snprintf (buf, sizeof (buf), "PC: 0x%08X", cpu->pc);
      mu_label (ctx, buf);

      if (mu_begin_treenode (ctx, "R0-R15"))
        {
          int cols[2] = { 250, 250 };
          for (int i = 0; i < 16; ++i)
            {
              if (i % 2 == 0)
                mu_layout_row (ctx, 2, cols, 18);
              snprintf (buf, sizeof (buf), "R%02d: 0x%08X", i, cpu->regs[i]);
              mu_label (ctx, buf);
            }
          mu_end_treenode (ctx);
        }

      if (mu_begin_treenode (ctx, "R16-R31"))
        {
          int cols[2] = { 250, 250 };
          for (int i = 16; i < 32; ++i)
            {
              if ((i - 16) % 2 == 0)
                mu_layout_row (ctx, 2, cols, 18);
              snprintf (buf, sizeof (buf), "R%02d: 0x%08X", i, cpu->regs[i]);
              mu_label (ctx, buf);
            }
          mu_end_treenode (ctx);
        }

      int button_widths[2];
      mu_layout_row (ctx, 2, button_widths, 28);
      button_widths[0] = mu_get_current_container (ctx)->body.w / 2 - 4;
      button_widths[1] = mu_get_current_container (ctx)->body.w / 2 - 4;
      mu_layout_row (ctx, 2, button_widths, 28);

      if (mu_button (ctx, "Step"))
        {
          MIPS32_run (cpu);
        }
      if (mu_button (ctx, "Reset PC"))
        {
          cpu->pc = 0;
        }

      mu_end_window (ctx);
    }
}

static void
window_write_interface (mu_Context *ctx, MIPS32_t *cpu, ui_state_t *ui)
{
  if (mu_begin_window_ex (ctx, "Write Interface", mu_rect (520, 10, 680, 680),
                          MU_OPT_NORESIZE))
    {
      char buf[128];

      mu_layout_row (ctx, 1, (int[]){ -1 }, 20);
      mu_label (ctx, "Target Address (hex):");
      mu_layout_row (ctx, 1, (int[]){ -1 }, 24);
      mu_textbox_ex (ctx, ui->addr_input, sizeof (ui->addr_input),
                     MU_OPT_ALIGNCENTER);

      mu_layout_row (ctx, 1, (int[]){ -1 }, 20);
      mu_label (ctx, "Write Type:");

      int write_type_widths[2];
      write_type_widths[0] = mu_get_current_container (ctx)->body.w / 2 - 4;
      write_type_widths[1] = mu_get_current_container (ctx)->body.w / 2 - 4;
      mu_layout_row (ctx, 2, write_type_widths, 28);

      if (mu_button (ctx, ui->write_type == WRITE_TYPE_VALUE ? "[*] Value"
                                                             : "[ ] Value"))
        {
          ui->write_type = WRITE_TYPE_VALUE;
        }
      if (mu_button (ctx, ui->write_type == WRITE_TYPE_INSTRUCTION
                              ? "[*] Instruction"
                              : "[ ] Instruction"))
        {
          ui->write_type = WRITE_TYPE_INSTRUCTION;
        }

      mu_layout_row (ctx, 1, (int[]){ -1 }, 8);
      mu_label (ctx, "");

      if (ui->write_type == WRITE_TYPE_VALUE)
        {
          mu_layout_row (ctx, 1, (int[]){ -1 }, 20);
          mu_label (ctx, "Value (hex):");
          mu_layout_row (ctx, 1, (int[]){ -1 }, 24);
          mu_textbox_ex (ctx, ui->value_input, sizeof (ui->value_input),
                         MU_OPT_ALIGNCENTER);

          int btn_widths[2];
          btn_widths[0] = mu_get_current_container (ctx)->body.w / 2 - 4;
          btn_widths[1] = mu_get_current_container (ctx)->body.w / 2 - 4;
          mu_layout_row (ctx, 2, btn_widths, 28);

          if (mu_button (ctx, "Write"))
            {
              u32 addr = 0, value = 0;
              if (strlen (ui->addr_input) > 0 && strlen (ui->value_input) > 0)
                {
                  sscanf (ui->addr_input, "%x", &addr);
                  sscanf (ui->value_input, "%x", &value);

                  if (addr + 4 <= 1024)
                    {
                      write_u32_be(cpu->memory, addr, value);
                      snprintf (ui->last_status, sizeof (ui->last_status),
                                "Wrote 0x%08X to 0x%X", value, addr);
                      ui->value_input[0] = '\0';
                    }
                  else
                    {
                      snprintf (ui->last_status, sizeof (ui->last_status),
                                "Error: Address out of bounds");
                    }
                }
              else
                {
                  snprintf (ui->last_status, sizeof (ui->last_status),
                            "Error: Fill all fields");
                }
            }

          if (mu_button (ctx, "Clear"))
            {
              ui->addr_input[0] = '\0';
              ui->value_input[0] = '\0';
              memset (ui->last_status, 0, sizeof (ui->last_status));
            }
        }
      else // WRITE_TYPE_INSTRUCTION
        {
          mu_layout_row (ctx, 1, (int[]){ -1 }, 20);
          mu_label (ctx, "Input Format:");
          int mode_widths[4];
          int w_total = mu_get_current_container (ctx)->body.w;
          for(int i=0;i<4;i++) mode_widths[i] = w_total / 4 - 4;
          mu_layout_row (ctx, 4, mode_widths, 24);
          int old_mode = ui->input_mode;
          if (mu_button(ctx, ui->input_mode == INPUT_MODE_HEX ? "[HEX]" : "HEX")) ui->input_mode = INPUT_MODE_HEX;
          if (mu_button(ctx, ui->input_mode == INPUT_MODE_BIN ? "[BIN]" : "BIN")) ui->input_mode = INPUT_MODE_BIN;
          if (mu_button(ctx, ui->input_mode == INPUT_MODE_DEC ? "[DEC]" : "DEC")) ui->input_mode = INPUT_MODE_DEC;
          if (mu_button(ctx, ui->input_mode == INPUT_MODE_OCT ? "[OCT]" : "OCT")) ui->input_mode = INPUT_MODE_OCT;
          if (old_mode != ui->input_mode) convert_fields(ui, old_mode, ui->input_mode);

          mu_layout_row (ctx, 1, (int[]){ -1 }, 20);
          mu_label (ctx, "Instruction Type:");
          int instr_widths[3];
          instr_widths[0] = mu_get_current_container (ctx)->body.w / 3 - 3;
          instr_widths[1] = mu_get_current_container (ctx)->body.w / 3 - 3;
          instr_widths[2] = mu_get_current_container (ctx)->body.w / 3 - 3;
          mu_layout_row (ctx, 3, instr_widths, 28);
          if (mu_button (ctx, ui->instr_type == INSTR_TYPE_R ? "[*] R" : "[ ] R")) ui->instr_type = INSTR_TYPE_R;
          if (mu_button (ctx, ui->instr_type == INSTR_TYPE_I ? "[*] I" : "[ ] I")) ui->instr_type = INSTR_TYPE_I;
          if (mu_button (ctx, ui->instr_type == INSTR_TYPE_J ? "[*] J" : "[ ] J")) ui->instr_type = INSTR_TYPE_J;

          mu_layout_row (ctx, 1, (int[]){ -1 }, 8);
          mu_label (ctx, "");

          if (ui->instr_type == INSTR_TYPE_R)
            {
              ui_field(ctx, "Opcode:", ui->r_opcode, sizeof(ui->r_opcode), ui->input_mode, 6);
              ui_field(ctx, "RS:", ui->r_rs, sizeof(ui->r_rs), ui->input_mode, 5);
              ui_field(ctx, "RT:", ui->r_rt, sizeof(ui->r_rt), ui->input_mode, 5);
              ui_field(ctx, "RD:", ui->r_rd, sizeof(ui->r_rd), ui->input_mode, 5);
              ui_field(ctx, "SHAMT:", ui->r_shamt, sizeof(ui->r_shamt), ui->input_mode, 5);
              ui_field(ctx, "FUNC:", ui->r_func, sizeof(ui->r_func), ui->input_mode, 6);
            }
          else if (ui->instr_type == INSTR_TYPE_I)
            {
              ui_field(ctx, "Opcode:", ui->i_opcode, sizeof(ui->i_opcode), ui->input_mode, 6);
              ui_field(ctx, "RS:", ui->i_rs, sizeof(ui->i_rs), ui->input_mode, 5);
              ui_field(ctx, "RT:", ui->i_rt, sizeof(ui->i_rt), ui->input_mode, 5);
              ui_field(ctx, "IMM:", ui->i_imm, sizeof(ui->i_imm), ui->input_mode, 16);
            }
          else
            {
              ui_field(ctx, "Opcode:", ui->j_opcode, sizeof(ui->j_opcode), ui->input_mode, 6);
              ui_field(ctx, "Target:", ui->j_target, sizeof(ui->j_target), ui->input_mode, 26);
            }

          mu_layout_row (ctx, 1, (int[]){ -1 }, 8);
          mu_label (ctx, "");

          int btn_widths[2];
          btn_widths[0] = mu_get_current_container (ctx)->body.w / 2 - 4;
          btn_widths[1] = mu_get_current_container (ctx)->body.w / 2 - 4;
          mu_layout_row (ctx, 2, btn_widths, 28);

          if (mu_button (ctx, "Write Instr"))
            {
              u32 addr = 0;
              if (strlen (ui->addr_input) > 0)
                {
                  sscanf (ui->addr_input, "%x", &addr);

                  if (addr + 4 <= 1024)
                    {
                      MIPS32_instruction_t instr_union;
                      instr_union.raw = 0; // Очищаем мусор

                      if (ui->instr_type == INSTR_TYPE_R)
                        {
                          instr_union.r.opcode = str_to_val (ui->r_opcode, ui->input_mode, NULL);
                          instr_union.r.rs     = str_to_val (ui->r_rs, ui->input_mode, NULL);
                          instr_union.r.rt     = str_to_val (ui->r_rt, ui->input_mode, NULL);
                          instr_union.r.rd     = str_to_val (ui->r_rd, ui->input_mode, NULL);
                          instr_union.r.shamt  = str_to_val (ui->r_shamt, ui->input_mode, NULL);
                          instr_union.r.funct  = str_to_val (ui->r_func, ui->input_mode, NULL);
                        }
                      else if (ui->instr_type == INSTR_TYPE_I)
                        {
                          instr_union.i.opcode = str_to_val (ui->i_opcode, ui->input_mode, NULL);
                          instr_union.i.rs     = str_to_val (ui->i_rs, ui->input_mode, NULL);
                          instr_union.i.rt     = str_to_val (ui->i_rt, ui->input_mode, NULL);
                          instr_union.i.imm    = str_to_val (ui->i_imm, ui->input_mode, NULL);
                        }
                      else
                        {
                          instr_union.j.opcode  = str_to_val (ui->j_opcode, ui->input_mode, NULL);
                          instr_union.j.address = str_to_val (ui->j_target, ui->input_mode, NULL);
                        }
                      write_u32_be(cpu->memory, addr, instr_union.raw);

                      snprintf (ui->last_status, sizeof (ui->last_status),
                                "Wrote Instr 0x%08X", instr_union.raw);
                    }
                  else
                    {
                      snprintf (ui->last_status, sizeof (ui->last_status),
                                "Error: Address out of bounds");
                    }
                }
              else
                {
                  snprintf (ui->last_status, sizeof (ui->last_status),
                            "Error: Set address");
                }
            }

          if (mu_button (ctx, "Clear"))
            {
              ui->addr_input[0] = '\0';
              memset (ui->last_status, 0, sizeof (ui->last_status));
            }
        }

      mu_layout_row (ctx, 1, (int[]){ -1 }, 8);
      mu_label (ctx, "");

      mu_layout_row (ctx, 1, (int[]){ -1 }, 20);
      mu_label (ctx, ui->last_status);

      mu_end_window (ctx);
    }
}


static void
window_memory_dump (mu_Context *ctx, MIPS32_t *cpu, ui_state_t *ui)
{
  if (mu_begin_window_ex (ctx, "Memory Dump", mu_rect (1210, 10, 700, 680),
                          MU_OPT_NORESIZE))
    {

      mu_layout_row (ctx, 1, (int[]){ -1 }, 18);
      mu_label (
          ctx,
          "Offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII");

      mu_layout_row (ctx, 1, (int[]){ -1 }, -1);

      mu_begin_panel (ctx, "##mem_scroll");
      mu_layout_row (ctx, 1, (int[]){ -1 }, 16);

      for (int addr = 0; addr < 512; addr += 16)
        {
          char hex_line[256];
          char *p = hex_line;

          p += sprintf (p, "%04x  ", addr);

          for (int i = 0; i < 16; i++)
            {
              u8 byte = cpu->memory[addr + i];
              p += sprintf (p, "%02x ", byte);
            }

          p += sprintf (p, " ");

          for (int i = 0; i < 16; i++)
            {
              u8 byte = cpu->memory[addr + i];
              if (byte >= 32 && byte < 127)
                {
                  p += sprintf (p, "%c", (char)byte);
                }
              else
                {
                  p += sprintf (p, ".");
                }
            }

          mu_label (ctx, hex_line);
        }

      mu_end_panel (ctx);

      mu_end_window (ctx);
    }
}

i32
main (int argc, char **argv)
{
  MIPS32_t *cpu = MIPS32_init (NULL);
  cpu->memory = malloc (1024);
  memset (cpu->memory, 0, 1024);

  MIPS32_instruction_i_type_t *ins
      = (MIPS32_instruction_i_type_t *)cpu->memory;
  ins->opcode = 0b001000;
  ins->rs = 13;
  ins->rt = 1;
  ins->imm = 0xAE;

  if (!SDL_Init (SDL_INIT_VIDEO))
    return 1;

  SDL_Window *window = SDL_CreateWindow ("MIPS32 Emulator",
                                         1920, 720, SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer (window, NULL);

  mu_Context ctx = { 0 };
  mu_init (&ctx);
  ctx.text_width = text_width;
  ctx.text_height = text_height;

  SDL_StartTextInput (window);

  ui_state_t ui_state = { 0 };
  ui_state.write_type = WRITE_TYPE_VALUE;
  ui_state.instr_type = INSTR_TYPE_R;

  int running = 1;

  while (running)
    {
      SDL_Event e;

      mu_begin (&ctx);

      while (SDL_PollEvent (&e))
        {
          if (e.type == SDL_EVENT_QUIT)
            running = 0;

          switch (e.type)
            {
            case SDL_EVENT_MOUSE_MOTION:
              mu_input_mousemove (&ctx, (int)e.motion.x, (int)e.motion.y);
              break;

            case SDL_EVENT_MOUSE_WHEEL:
              mu_input_scroll (&ctx, 0, (int)(e.wheel.y * -30));
              break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
              mu_input_mousedown (&ctx, (int)e.button.x, (int)e.button.y,
                                  (1 << (e.button.button - 1)));
              break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
              mu_input_mouseup (&ctx, (int)e.button.x, (int)e.button.y,
                                (1 << (e.button.button - 1)));
              break;

            case SDL_EVENT_TEXT_INPUT:
              if (e.text.text && e.text.text[0])
                {
                  mu_input_text (&ctx, e.text.text);
                }
              break;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
              handle_keyboard_input (&ctx, &e);
              break;
            }
        }

      window_registers (&ctx, cpu, &ui_state);
      window_write_interface (&ctx, cpu, &ui_state);
      window_memory_dump (&ctx, cpu, &ui_state);

      mu_end (&ctx);

      SDL_SetRenderDrawColor (renderer, 30, 30, 30, 255);
      SDL_RenderClear (renderer);
      render_microui (renderer, &ctx);
      SDL_RenderPresent (renderer);
    }

  SDL_StopTextInput (window);
  free (cpu->memory);
  MIPS32_free (cpu);
  SDL_DestroyRenderer (renderer);
  SDL_DestroyWindow (window);
  SDL_Quit ();

  return 0;
}