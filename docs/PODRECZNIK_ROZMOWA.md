# Cube Solver — podręcznik techniczny pod rozmowę o staż C++

Dokument przygotowany do szybkiego odświeżenia całego projektu przed rozmową.
Cel: umieć płynnie opowiedzieć *co*, *jak* i *dlaczego* — oraz odpowiedzieć na pytania techniczne.

---

## 0. Elevator pitch (30–60 sekund)

> Zrobiłem aplikację w **C++17** z **OpenCV**, która przez kamerę skanuje fizyczną kostkę Rubika (6 ścian × 9 naklejek), rozpoznaje kolory w przestrzeni **HSV** z algorytmem **capacity-aware clustering** (dokładnie 9 naklejek na kolor), buduje 54-znakowy string w formacie **Kociemba**, a rozwiązanie liczy przez most do Pythona (`popen` + biblioteka `kociemba`). Użytkownik dostaje albo klasyczną notację ruchów (U, R', F2…), albo lokalizowane instrukcje krok po kroku na ekranie. Build: **CMake**, zależności: OpenCV 4.x + Python `kociemba`.

**Słowa kluczowe do CV/rozmowy:** C++17, OpenCV, computer vision, HSV, clustering, CMake, IPC/subprocess, algorytm Kociemba, real-time UI.

---

## 1. Co robi program — ścieżka użytkownika

1. Start → wybór języka (EN/PL) → wybór trybu (notacja vs instrukcje).
2. Otwiera kamerę, rysuje siatkę 3×3 na podglądzie na żywo.
3. Użytkownik przykłada ściany w ustalonej kolejności i naciska **SPACE** (6 razy).
4. Po 6. skanie: clustering kolorów → string Kociemba → solve.
5. Jeśli stan niemożliwy (złe światło / błąd skanu) → komunikat i ponowny skan.
6. Jeśli OK:
   - **Tryb 1:** wypisuje rozwiązanie w notacji (np. `R U R' U'`).
   - **Tryb 2:** pokazuje kroki na nakładce wideo (SPACE = następny krok).

**Orientacja kostki przy rozwiązywaniu:** pomarańczowy środek z przodu (F), żółty na górze (U).

---

## 2. Architektura — mapa projektu

```
Cube_Solver/
├── CMakeLists.txt          # 3 targety: Cube_Solver, TestWebcam, Calibrate
├── include/                # publiczne headery
│   ├── webcam_capture.h
│   ├── grid_overlay.h
│   ├── color_clustering.h
│   ├── kociemba_string.h
│   └── kociemba.h
├── src/
│   ├── main.cpp            # orchestracja: UI, pętla, tryby, i18n
│   ├── vision/             # warstwa CV / kamera
│   │   ├── webcam_capture.cpp
│   │   ├── grid_overlay.cpp
│   │   ├── test_webcam.cpp       # osobny binary do testu kamery
│   │   └── color_calibration.cpp # osobny binary do debugu HSV
│   └── solver/             # warstwa rozpoznawania + solve
│       ├── color_clustering.cpp
│       ├── kociemba_string.cpp
│       └── kociemba.cpp
└── docs/media/demo.gif
```

### Warstwy (jak o tym mówić)

| Warstwa | Odpowiedzialność | Pliki |
|--------|------------------|-------|
| **I/O kamery** | otwarcie / klatka / RAII release | `WebcamCapture` |
| **UI / overlay** | siatka 3×3, labele, sampling HSV | `GridOverlay` |
| **Recognition** | przypisanie 54 stickers → 6 kolorów | `ColorClustering` |
| **Serialization** | mapa kolorów → string 54 znaków | `buildKociembaString` |
| **Solve bridge** | C++ → Python `kociemba.solve` | `run_python_kociemba` |
| **App / UX** | język, tryby, pętla, retry | `main.cpp` |

To jest klasyczny **pipeline**: Capture → Sample → Cluster → Encode → Solve → Present.

---

## 3. Build system (CMake) — co warto wiedzieć

```cmake
cmake_minimum_required(VERSION 3.10)
project(Cube_Solver)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)
```

Trzy executable'y z jednego `CMakeLists.txt`:

| Target | Źródła | Po co |
|--------|--------|-------|
| `Cube_Solver` | main + vision + solver | główna aplikacja |
| `TestWebcam` | test_webcam + webcam_capture | szybki smoke-test kamery |
| `Calibrate` | color_calibration + webcam | kliknięcie w piksel → odczyt HSV |

**Linkowanie:** `target_link_libraries(... ${OpenCV_LIBS})` — OpenCV przez `find_package`.

**Dlaczego C++17?** Używane m.in.: structured bindings (`for (const auto& [dist, stickerIdx, face] : ...)`), `std::map`/`vector`, ogólna nowoczesna składnia. Nie ma coroutines / ranges — 17 wystarczy i jest szeroko wspierany.

**Jak zbudować (na rozmowie):**
```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./Cube_Solver
```

---

## 4. Pipeline szczegółowo (krok po kroku)

### 4.1 Capture — `WebcamCapture`

```cpp
class WebcamCapture {
    cv::VideoCapture cap;
    cv::Mat frame;
    // open w ctor, release w dtor (RAII)
    bool captureFrame();  // cap.read(frame)
    cv::Mat getFrame();
};
```

- Konstruktor: `cap.open(cameraIndex)` (domyślnie 0).
- Destruktor: `cap.release()` — **RAII**, zasób kamery zwalniany automatycznie.
- `captureFrame()` → `cap.read(frame)` → `bool`.
- `getFrame()` zwraca `cv::Mat` **by value** (OpenCV używa reference counting — tani copy, shared data).

**Na rozmowie:** „Owrapowałem `VideoCapture` w klasę z RAII, żeby nie wyciekał uchwyt kamery.”

### 4.2 Preview loop — `main.cpp`

Główna pętla (uproszczenie):

```
while (true):
  captureFrame()
  flip(frame, 1)          # lustro — naturalniejsze dla użytkownika
  drawGrid(...)           # siatka + instrukcje skanu
  [opcjonalnie] draw steps overlay
  imshow(...)
  waitKey(1)
  SPACE → captureFace() i przejście do następnej ściany
  ESC → wyjście
```

**Kolejność skanowania ścian** (twardo zakodowana w `main`):

| # | Środek skanowanej ściany (`colorCenter`) | Kolor „góry” do orientacji (`colorAbove`) |
|---|------------------------------------------|-------------------------------------------|
| 1 | Yellow | Blue |
| 2 | Green | Yellow |
| 3 | Orange | Yellow |
| 4 | Blue | Yellow |
| 5 | Red | Yellow |
| 6 | White | Red |

Po White → `scanningDone = true`, overlay chowa się (`setVisible(false)`), kamera dalej działa.

**Dlaczego `colorAbove`?** Żeby użytkownik trzymał kostkę w spójnej orientacji — bez tego kolejność faceletów na ścianie byłaby nierozróżnialna / przekręcona względem modelu Kociemba.

### 4.3 Sampling — `GridOverlay::captureFace()`

Dla każdej z 9 komórek siatki:

1. Frame BGR → HSV (`cvtColor`).
2. Środek komórki: patch **5×5** pikseli.
3. Średnia HSV (`cv::mean`) → `cv::Vec3f`.
4. **Mirror kolumn:** `mirroredCol = 2 - col` — bo wcześniej robi się `cv::flip(frame, 1)` (lustro), więc sampling musi odwrócić kolumny, żeby indeksy stickers odpowiadały rzeczywistej kostce, nie lustrzanemu obrazowi.

Zwraca `vector<Vec3f>` długości 9, indeks 4 = **center** (kotwica koloru ściany).

**Dlaczego HSV a nie RGB?**
- Hue (odcień) jest względnie stabilny przy zmianach jasności.
- Saturation / Value oddzielają „czy to kolor” od „jak jasno”.
- Przy kostce (żółty/biały/pomarańcz) RGB progi łatwo się rozjeżdżają przy złym świetle.

**Dlaczego średnia z patcha, nie pojedynczy piksel?** Redukcja szumu / glare / artefaktów kamery.

### 4.4 Clustering — `ColorClustering::assignColorsToCenters`

To **najciekawsza część algorytmiczna** projektu — warto znać na pamięć.

#### Idea

Mamy 54 próbki HSV. Środki ścian (sticker #4 każdej ściany) to **stałe kotwice kolorów** (Yellow/Orange/Red/Green/Blue/White). Każdy sticker trzeba przypisać do jednego z 6 kolorów, z twardego constraintu: **dokładnie 9 naklejek na kolor** (fizyczna kostka).

#### Algorytm (greedy capacity-aware)

1. Zbierz 54 stickers w ustalonej kolejności ścian: Yellow, Orange, Red, Green, Blue, White.
2. Dla każdej ściany: `faceCenters[face] = samples[4]` (środek).
3. Policz **wszystkie** pary (sticker, kolor) → 54×6 = **324** dystanse.
4. Sortuj rosnąco po dystansie.
5. Greedy: idź od najbliższych; jeśli sticker wolny **i** kolor ma jeszcze slot (`count < 9`) → przypisz.
6. Złóż wynik: `map<face, vector<9 color names>>`.
7. Korekty orientacji (patrz niżej).
8. Warning jeśli jakiś kolor ≠ 9 (nie powinno się zdarzyć przy capacity).

#### Metryka HSV

```cpp
float dh = min(|Ha-Hb|, 180 - |Ha-Hb|);  // Hue jest kołowy! (0..179 w OpenCV)
float ds = |Sa-Sb|;
float dv = |Va-Vb|;
return sqrt(dh² + ds² + dv²);
```

**Kluczowy detail na rozmowę:** Hue jest okresowy (czerwień koło 0 i ~179). Bez `min(..., 180-...)` dystans czerwony↔czerwony mógłby być ogromny.

#### Rotacje korekcyjne (ważne!)

Po clusterze:

- **Yellow (U):** obrót **90° CW** w reprezentacji 3×3.
- **White (D):** obrót **180°**.

Dlaczego? Kolejność skanowania + sposób trzymania kostki nie pokrywa się 1:1 z konwencją faceletów Kociemba. Te rotacje wyrównują indeksy stickers do oczekiwanego układu solvera. Bez nich string byłby „legalny kolorystycznie”, ale **geometrycznie zły** → ValueError / nonsensowne rozwiązanie.

Indeksy 3×3 (row-major):
```
0 1 2
3 4 5
6 7 8
```
Rotacja 90° CW Yellow: `0←6, 1←3, 2←0, 3←7, 5←1, 6←8, 7←5, 8←2` (4 zostaje).

### 4.5 Encoding — `buildKociembaString`

Mapowanie kolor → litera (środek = tożsamość ściany w notacji):

| Kolor | Litera | Ściana |
|-------|--------|--------|
| Yellow | U | Up |
| Blue | R | Right |
| Orange | F | Front |
| White | D | Down |
| Green | L | Left |
| Red | B | Back |

**Kolejność ścian w stringu (Kociemba):** `U R F D L B` = Yellow, Blue, Orange, White, Green, Red.

Wynik: string długości **54** (6×9), np. `UUUUUUUUURRR...`.

### 4.6 Solve — `run_python_kociemba`

```cpp
string command = "python3 -c \"import kociemba; print(kociemba.solve('" + s + "'))\" 2>/dev/null";
FILE* pipe = popen(command.c_str(), "r");
// czytaj stdout do stringa, pclose, trim '\n'
```

- **IPC:** `popen` — uruchamia proces potomny, czyta jego stdout.
- `2>/dev/null` — tłumi stderr Pythona (ValueError przy złym stanie nie zaśmieca UX; w C++ sprawdzasz pusty wynik / `"ValueError"` w stringu).
- Biblioteka Python `kociemba` implementuje **Two-Phase Algorithm** Herberta Kociemby (wariacja IDA* po przestrzeni stanów kostki; typowo rozwiązanie w ~20 ruchach).

**Dlaczego nie native C++ solver?**
Szybka, poprawna implementacja two-phase to dużo kodu (tablice move/prune). Most do sprawdzonej libki = pragmatyzm: CV i clustering w C++, solve jako black-box. Na rozmowie podkreśl **świadomy trade-off**: mniej zależności natywnych vs narzut procesu + zależność od Pythona.

### 4.7 Presentation

**Tryb 1 (notacja):** wypis `Solution: R U R' ...` + przypomnienie orientacji.

**Tryb 2 (instrukcje):** parser rozwiązania:
- litery `UDRLFB` = ruch,
- `'` = trzy razy zgodnie (bo 270° = 3×90° CW — unika „przeciwnie do wskazówek”),
- `2` = dwa razy.

Mapowanie litery → kolor środka (żeby laik kręcił właściwą ścianą):
U→Yellow, D→White, L→Green, R→Blue, F→Orange, B→Red.

Instrukcje rysowane na klatce (`putText` + czarny prostokąt tła) albo po zamknięciu kamery w konsoli (ENTER).

---

## 5. OpenCV — co konkretnie używasz

| API | Gdzie | Po co |
|-----|-------|-------|
| `VideoCapture` | WebcamCapture | kamera |
| `Mat` | wszędzie | obraz / HSV |
| `cvtColor(..., BGR2HSV)` | captureFace, Calibrate | przestrzeń kolorów |
| `mean` | captureFace | średnia patcha |
| `line`, `putText`, `rectangle`, `getTextSize` | GridOverlay, main | UI overlay |
| `flip(..., 1)` | main | lustro horizontal |
| `imshow` / `waitKey` | main | okno + input |
| `destroyAllWindows` | main | cleanup |
| `utils::logging::setLogLevel(ERROR)` | main | cisza logów OpenCV |
| `setMouseCallback` | Calibrate | debug HSV kliknięciem |
| `nth_element` | Calibrate | mediana H/S/V całej klatki |

**BGR vs RGB:** OpenCV domyślnie BGR — scalars kolorów w kodzie (`Scalar(0,255,255)` = żółty) są w BGR.

---

## 6. Koncepty C++, które warto wyciągnąć na rozmowie

To jest projekt studencki ~800 LOC — nie udawaj enterprise, ale pokaż świadomość:

1. **RAII** — `WebcamCapture` zwalnia kamerę w destruktorze.
2. **Separacja odpowiedzialności** — vision vs solver vs UI orchestration.
3. **Referencje zamiast kopiowania ciężkich obiektów** — `WebcamCapture&` w `GridOverlay`.
4. **Header / source split** — deklaracje w `include/`, definicje w `src/`.
5. **CMake multi-target** — główna app + narzędzia diagnostyczne.
6. **STL:** `vector`, `map`, `tuple`, `sort`, structured bindings C++17.
7. **IPC / subprocess** — `popen` / `pclose`, budowa komendy, czytanie pipe.
8. **Obsługa błędów UX:** retry skanu przy invalid cube zamiast crasha.
9. **i18n proste:** `language == 1/2` + `translateColorName`.
10. **Computer vision praktyczne:** HSV, circular hue distance, capacity constraints.

### Czego NIE ma (i jak o tym mówić szczerze)

- Brak unit testów / CI.
- Brak abstract interfaces / DI — prosty proceduralny flow w `main`.
- Solver nie jest natywny C++.
- Clustering jest greedy (nie optymalny assignment / Hungarian) — działa bo constraint 9/kolor + dobre kotwice ze środków.
- Brak kalibracji kolorów w runtime (jest tool `Calibrate`, ale główna app nie ładuje profilu).
- `main.cpp` jest dość monolityczny (~300 LOC) — naturalny next step: wyciągnąć stany FSM (Scanning / Solving / Instructing).

**Fraza:** „Przy kolejnej iteracji wydzieliłbym state machine i natywny solver / bibliotekę C++, oraz dodał testy clusteringu na nagranych samplingach.”

---

## 7. Algorytm Kociemba — enough for interview

Kostka 3×3: ~43 tryliony stanów. Brute-force BFS nierealny w czasie rzeczywistym.

**Two-Phase Algorithm (Kociemba):**
- **Phase 1:** doprowadź kostkę do podgrupy G1 (krawędzie zorientowane, narożniki zorientowane, środki boczne w „pasie”) — ruchy ograniczone.
- **Phase 2:** z G1 do solved, używając tylko ruchów zachowujących G1.
- Używa tablic odległości (pruning tables) + IDA*/DFS z boundami.

W projekcie: **nie implementujesz tego** — wołasz gotową libkę. Ważne, żeby wiedzieć *czym jest* i *dlaczego* string 54 znaków w kolejności URFDLB.

Walidacja: `kociemba.solve` rzuca ValueError, gdy układ jest fizycznie niemożliwy (zły parity / złe kolory narożników) — stąd retry skanu.

---

## 8. Narzędzia pomocnicze

### `TestWebcam`
Minimalna pętla: capture → show → ESC. Sprawdza, że OpenCV widzi kamerę.

### `Calibrate`
- Okno + mouse callback.
- LPM na piksel → print HSV.
- Dodatkowo zbiera mediany H/S/V całej klatki (`nth_element`).
- Służyło do debugowania rozpoznawania kolorów przy złym świetle / ustawianiu intuicji HSV.

---

## 9. Historia rozwoju (z gita — narracja „jak powstawało”)

Chronologicznie (warto umieć opowiedzieć ewolucję):

1. Struktura projektu + CMake  
2. Webcam capture + test  
3. Pierwsze detekcje kolorów (thresholdy — problematyczne)  
4. Grid overlay + labelki  
5. Skanowanie ścian z siatki  
6. **Color clustering** + porządki  
7. Most do Python Kociemba  
8. „final v1”  
9. Lustro kamery + język polski  
10. UX: instrukcje on-screen, demo.gif, README, license MIT  

**Story na rozmowę:** „Zaczynałem od progów RGB/HSV — padało przy świetle. Przeszedłem na clustering względem środków ścian z limitem 9, i dopiero wtedy solve stał się wiarygodny.”

---

## 10. Typowe pytania z rozmowy + gotowe odpowiedzi

### „Opowiedz o projekcie”
Użyj elevator pitch z §0 + dodaj: pipeline Capture→Cluster→Kociemba→UI, C++17/OpenCV/CMake, ~800 LOC.

### „Jaki był najtrudniejszy problem?”
**Rozpoznawanie kolorów przy zmiennym świetle** + **orientacja faceletów** względem konwencji Kociemba (rotacje U/D). Mirror + `mirroredCol` też łatwo pomylić.

### „Dlaczego HSV?”
Oddzielenie hue od jasności; circular distance na H; lepsza odporność niż RGB thresholds.

### „Dlaczego nie K-means?”
Mamy *znane* centroidy (środki ścian) i twardy constraint cardinality. Greedy po posortowanych dystansach z capacity jest prostszy, deterministyczny i wystarczający. K-means nie gwarantuje 9/klaster bez modyfikacji.

### „Co to jest RAII w Twoim kodzie?”
Destruktor `WebcamCapture` robi `cap.release()`.

### „Jak komunikujesz się z Pythonem?”
`popen` z `python3 -c "..."`, czytanie stdout, stderr wyciszony, wynik string z ruchami.

### „Co byś poprawił?”
- State machine zamiast flag w `main`
- Testy clusteringu na fixture'ach HSV
- Opcjonalnie natywny solver C++ / libcube
- Kalibracja kolorów / white balance
- Lepsza walidacja wejścia (`cin` bez failbit handling)

### „Złożoność clusteringu?”
324 dystanse + sort O(n log n) przy n=324 → praktycznie stałe, mikrosekundy vs czas kamery/solve.

### „Bezpieczeństwo `popen`?”
String kostki to tylko `UDRLFB` z Twojego enkodera — nie user-controlled free text. Przy generalizacji: unikać sklejania shell command; lepiej `execve` z argumentami / biblioteka w-process.

---

## 11. Ściąga: stałe i mapowania

**Skan (kolejność center):** Yellow → Green → Orange → Blue → Red → White  

**Kociemba face order:** Yellow(U), Blue(R), Orange(F), White(D), Green(L), Red(B)  

**Color→char:** Y→U, G→L, O→F, B→R, R→B, W→D  

**Orientacja solve:** Orange front, Yellow up  

**Ruch `'` w trybie instrukcji:** „TRZY razy zgodnie” (nie „przeciwnie”)  

**Klawisze:** SPACE/ENTER = skan / next step; ESC = exit  

---

## 12. Mini-ściąga plików (co jest w którym)

| Plik | ~LOC | Jednym zdaniem |
|------|------|----------------|
| `main.cpp` | 299 | Język, tryby, pętla kamery, skan 6 ścian, solve, UI instrukcji |
| `grid_overlay.cpp` | 122 | Rysowanie siatki/labeli + sampling 9× HSV (z mirror) |
| `color_clustering.cpp` | 121 | HSV distance + greedy assign 9/kolor + rotacje U/D |
| `webcam_capture.cpp` | 33 | Wrapper VideoCapture (RAII) |
| `kociemba_string.cpp` | 26 | Map kolorów → string 54 |
| `kociemba.cpp` | 14 | popen → python kociemba |
| `color_calibration.cpp` | 57 | Debug tool HSV |
| `test_webcam.cpp` | 23 | Smoke test kamery |

---

## 13. Jak ćwiczyć opowiadanie (rekomendowana kolejność)

1. Elevator pitch na głos (60 s).  
2. Narysuj na kartce pipeline 6 bloków.  
3. Wyjaśnij clustering z capacity na przykładzie „dlaczego nie nearest-neighbor bez limitu”.  
4. Wyjaśnij string Kociemba i po co rotacje Yellow/White.  
5. Jedno zdanie o `popen` i trade-offie Python bridge.  
6. Jedno zdanie „co bym poprawił”.  

Jeśli potrafisz to bez zaglądania — jesteś gotowy na rozmowę o tym projekcie.

---

## 14. Szybki glossariusz

- **Facelet / sticker** — jedna naklejka (z 54).  
- **Center** — środkowy facelet ściany; definiuje kolor ściany (nie rusza się względnie względem innych środków przy legalnych ruchach 3×3… w standardowej kostce środki są stałe względem rdzenia).  
- **HSV** — Hue, Saturation, Value.  
- **Kociemba string** — 54 znaki, kolejność URFDLB, każdy znak = która ściana „koloru” jest na danym facelecie.  
- **Two-phase** — algorytm rozwiązywania przez podgrupę G1.  
- **RAII** — Resource Acquisition Is Initialization.  
- **Capacity-aware assignment** — przypisanie z limitem liczby elementów na klaster.

---

*Repo: Cube_Solver · C++17 · OpenCV · CMake · Python kociemba · MIT (Szostak21, 2025)*
