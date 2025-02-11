#include QMK_KEYBOARD_H

enum layer_names {
    _BASE
};

// このキーを押すと「+」、Shift押しながらだと「-」
enum custom_keycodes {
    MY_PLUS = SAFE_RANGE
};

#include "print.h" // uprintf 等を使う場合

// 1. カスタムの「シフト時の動作」を管理するテーブルを用意
typedef struct {
    uint16_t base_kc;       // 物理キーコード (例: KC_2)
    const char *mod_str; // Shift押下中に送る文字列 (例: "_")
} mod_override_t;

// ここで置き換えたいキーと、そのShift時の文字を羅列
static const mod_override_t shift_overrides[] = {
    {KC_1, "!"},
    {KC_2, "_"},
    {KC_3, "#"},
    {KC_4, "?"},
    {KC_5, "%"},
    {KC_6, "^"},
    {KC_7, "&"},
    {KC_8, "|"},
    {KC_9, "\\"}, // バックスラッシュ -> "\\" とエスケープ
    {KC_0, ""},   // Shift+0 → 何もしない例
    {KC_EQL, ""}, // Shift+= → 何もしない
    {KC_SLSH, "*"},
    {KC_SPC, "  "},
};

static const mod_override_t alt_overrides[] = {
    {KC_Y,  "{"},
    {KC_U,  "}"},
    {KC_H,  "("},
    {KC_J,  ")"},
    {KC_N,  "["},
    {KC_M,  "]"},
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_BASE] = LAYOUT(
    KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_EQL, KC_BSPC,
    KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, MY_PLUS, KC_SLSH,
    KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_UP, KC_ENT,
    KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_LEFT, KC_DOWN, KC_RGHT,
    KC_BTN1, KC_ESC,
    OSM(MOD_LSFT),  // **通常 Shift / 2回押しで固定**
    OSM(MOD_LCTL),  // **通常 Ctrl / 2回押しで固定**
    OSM(MOD_LALT),  // **通常 Alt / 2回押しで固定**
    KC_SPC
)
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true; // キーが離されたときは何もしない
    }

    bool is_shifted = (get_mods() & MOD_MASK_SHIFT);
    bool is_alted = (get_mods() & MOD_MASK_ALT);

    //
    // (A) 特殊キー: MY_PLUS (テンキーの `+` を押したことにする)
    //
    if (keycode == MY_PLUS) {
        if (is_shifted) {
            // Shift + MY_PLUS -> "-"
            uint8_t shift_mods = get_mods() & MOD_MASK_SHIFT;
            del_mods(shift_mods);
            tap_code(KC_MINS);  // 「-」を入力
            set_mods(shift_mods);
        } else {
            // 単押し → 「+」を送る (Shift + KC_EQL)
            register_code(KC_LSFT);
            tap_code(KC_EQL);
            unregister_code(KC_LSFT);
        }
        return false;
    }

    //
    // (B) Shift+キーの置き換え (shift_overrides)
    //
    if (is_shifted) {
        for (uint8_t i = 0; i < sizeof(shift_overrides) / sizeof(shift_overrides[0]); i++) {
            if (shift_overrides[i].base_kc == keycode) {
                uint8_t current_mods = get_mods();
                del_mods(MOD_MASK_SHIFT);
                send_string(shift_overrides[i].mod_str);
                set_mods(current_mods);
                return false;
            }
        }
    }

    //
    // (C) Alt+キーの置き換え (alt_overrides)
    //
    if (is_alted) {
        for (uint8_t i = 0; i < sizeof(alt_overrides) / sizeof(alt_overrides[0]); i++) {
            if (alt_overrides[i].base_kc == keycode) {
                uint8_t current_mods = get_mods();
                del_mods(MOD_MASK_ALT);
                send_string(alt_overrides[i].mod_str);
                set_mods(current_mods);
                return false;
            }
        }
    }

    return true;
}
