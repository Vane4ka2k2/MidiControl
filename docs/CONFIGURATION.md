# Руководство по конфигурации config.json ⚙️

Конфигурационный файл `config.json` располагается в корневой папке приложения `MidiControl` и управляет поведением программы, назначением MIDI-каналов, горячих клавиш и всплывающего OSD-индикатора.

---

## 📄 Структура JSON файла

Пример полного файла `config.json`:

```json
{
  "device_name": "Minilab",
  "show_osd": true,
  "osd_duration_ms": 1000,
  "faders": {
    "fader_1": {
      "cc_arturia": 82,
      "cc_daw": 14,
      "type": "master_volume",
      "label": "Общая громкость"
    }
  },
  "encoders": {
    "encoder_1": { "cc": 74, "action": "seek_media", "label": "Перемотка трека" },
    "encoder_2": { "cc": 71, "action": "zoom_browser", "label": "Масштаб страницы" }
  },
  "pads": {
    "pad_1": { "cc": 102, "note": 36, "action": "master_mute", "label": "Отключить звук" },
    "pad_2": { "cc": 103, "note": 37, "action": "smart_ducking", "label": "Приглушение" },
    "pad_3": { "cc": 104, "note": 38, "action": "media_prev", "label": "Предыдущий трек" },
    "pad_4": { "cc": 105, "note": 39, "action": "media_play_pause", "label": "Пауза / Воспроизведение" },
    "pad_5": { "cc": 106, "note": 40, "action": "media_next", "label": "Следующий трек" },
    "pad_6": { "cc": 107, "note": 41, "action": "launch_telegram", "label": "Запуск Telegram" },
    "pad_7": { "cc": 108, "note": 42, "action": "show_desktop", "label": "Рабочий стол (Win+D)" },
    "pad_8": { "cc": 109, "note": 43, "action": "snipping_tool", "label": "Скриншот" }
  }
}
```

---

## 🛠 Описание параметров

### 1. Глобальные настройки (`Global`)

| Параметр | Тип | Значение по умолчанию | Описание |
| :--- | :--- | :--- | :--- |
| `device_name` | `string` | `"Minilab"` | Подстрока для автопоиска MIDI-порта в системе Windows |
| `show_osd` | `boolean` | `true` | Показывать ли плашку OSD над панелью задач |
| `osd_duration_ms` | `integer` | `1000` | Время показа плашки OSD в миллисекундах |

---

### 2. Секция фейдеров (`faders`)

Описывает физические ползунки на клавиатуре.

```json
"fader_1": {
  "cc_arturia": 82,
  "cc_daw": 14,
  "type": "master_volume",
  "label": "Общая громкость",
  "apps": ["chrome.exe", "spotify.exe"]
}
```

- **`cc_arturia`**: Номер Control Change (CC) в режиме Arturia/User Preset (по умолчанию `82` для Fader 1).
- **`cc_daw`**: Номер CC в режиме DAWs Preset (по умолчанию `14` для Fader 1).
- **`type`**:
  - `"master_volume"` — Регулировка общей мастер-громкости Windows.
  - `"app_volume"` — Регулировка громкости целевой группы процессов из списка `apps`.
- **`label`**: Текстовое название, выводящееся на OSD.
- **`apps`** *(опционально)*: Список `.exe` имён процессов для типа `"app_volume"`.

---

### 3. Секция энкодеров (`encoders`)

Описывает вращающиеся ручки-энкодеры (Encoders 1–2).

| Энкодер | CC | Команда (`action`) | Описание действия |
| :--- | :--- | :--- | :--- |
| `encoder_1` | `74` | `seek_media` | Перемотка воспроизведения (Вперед при `CC > 64`, Назад при `CC < 64`) |
| `encoder_2` | `71` | `zoom_browser` | Масштабирование сайтов (`Ctrl+Plus` при `CC > 64`, `Ctrl+Minus` при `CC < 64`) |

---

### 4. Секция пэдов (`pads`)

Описывает 8 физических нажатий пэдов (Pads 1–8). Каждая кнопка может посылать как события MIDI Note (`Note On/Off`), так и MIDI Control Change (`CC`).

#### Доступные действия (`action`):

| Action Identifier | Описание действия | Подробности |
| :--- | :--- | :--- |
| `cycle_audio_device` | Переключение аудиовыхода | Цикличный выбор дефолтного устройства WASAPI (Наушники ↔ Динамики) |
| `show_desktop` | Сворачивание окон | Вызов системной комбинации `Win + D` |
| `launch_telegram` | Запуск Telegram | Открытие URI-протокола `tg://` |
| `media_play_pause` | Пауза / Плей | Отправка медиа-клавиши `VK_MEDIA_PLAY_PAUSE` |
| `media_next` | Следующий трек | Отправка медиа-клавиши `VK_MEDIA_NEXT_TRACK` |
| `media_prev` | Предыдущий трек | Отправка медиа-клавиши `VK_MEDIA_PREV_TRACK` |
| `smart_ducking` | Приглушение звука | المгновенное снижение громкости до 15% / восстановление исходной |
| `snipping_tool` | Захватки экрана | Вызов приложения Ножницы (`Win + Shift + S`) |

---

## 💡 Резервный фолбэк (Fail-Safe)

Если файл `config.json` отсутствует, нечитаем или повреждён при запуске, класс `ConfigParser::SetDefaultConfig` автоматически применит эталонную конфигурацию для **Arturia Minilab 3**, предотвращая краш приложения.
