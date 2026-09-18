#ifndef INDEX_HTML_H
#define INDEX_HTML_H


const char INDEX_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width, initial-scale=1.0">


<title>VL53L5CX ToF Live</title>


<style>

/* =========================================================
   PAGE
   ========================================================= */

body
{
    margin: 0;

    padding: 20px;

    background: #111111;

    color: white;

    font-family: Arial, Helvetica, sans-serif;

    text-align: center;
}


/* =========================================================
   TITLE
   ========================================================= */

h1
{
    margin: 0 0 8px 0;

    font-size: 28px;
}


#connection
{
    margin-bottom: 20px;

    font-size: 16px;

    color: #cccccc;
}


/* =========================================================
   TOF GRID
   ========================================================= */

#grid
{
    display: grid;

    grid-template-columns:
        repeat(8, 80px);

    grid-template-rows:
        repeat(8, 80px);

    gap: 3px;

    width: max-content;

    margin-left: auto;

    margin-right: auto;
}


/* =========================================================
   GRID CELL
   ========================================================= */

.cell
{
    width: 80px;

    height: 80px;

    display: flex;

    align-items: center;

    justify-content: center;

    border-radius: 4px;

    font-size: 14px;

    font-weight: bold;

    color: white;

    background: #222222;

    box-sizing: border-box;
}


/* =========================================================
   INFORMATION
   ========================================================= */

#information
{
    margin-top: 20px;

    font-size: 17px;

    line-height: 1.8;
}


/* =========================================================
   MOBILE / SMALL SCREEN
   ========================================================= */

@media
(max-width: 750px)
{
    #grid
    {
        grid-template-columns:
            repeat(8, 10vw);

        grid-template-rows:
            repeat(8, 10vw);

        gap: 2px;
    }


    .cell
    {
        width: 10vw;

        height: 10vw;

        font-size: 10px;
    }
}

</style>

</head>


<body>


<h1>
    VL53L5CX LIVE DEPTH
</h1>


<div id="connection">
    Connecting...
</div>


<div id="grid">
</div>


<div id="information">

    Frame:
    <span id="frame">
        --
    </span>

    &nbsp;&nbsp;|&nbsp;&nbsp;

    Valid zones:
    <span id="valid">
        --
    </span>

</div>


<script>


// ============================================================
// CREATE 64 CELLS
// ============================================================

const grid =
    document.getElementById("grid");


for (
    let i = 0;
    i < 64;
    i++
)
{
    const cell =
        document.createElement("div");


    cell.className =
        "cell";


    cell.id =
        "cell" + i;


    cell.innerText =
        "--";


    grid.appendChild(
        cell
    );
}


// ============================================================
// DEPTH → COLOUR
// ============================================================
//
// 0 mm      → red
// ~500 mm   → yellow
// ~1000 mm  → green
// ~1500 mm  → cyan
// ~2000 mm  → blue
//
// Values above 2000 mm are clamped.
//

function depthColor(
    distance
)
{
    if (
        distance <= 0
    )
    {
        return "#222222";
    }


    let d =
        Math.max(
            0,
            Math.min(
                2000,
                distance
            )
        );


    let ratio =
        d / 2000.0;


    // HSV-like rainbow mapping.
    //
    // hue:
    // 0.0   = red
    // 0.66  = blue

    let hue =
        (1.0 - ratio) * 240.0;


    return (
        "hsl(" +
        hue +
        ", 100%, 50%)"
    );
}


// ============================================================
// UPDATE CONNECTION STATUS
// ============================================================

function setConnection(
    connected
)
{
    const element =
        document.getElementById(
            "connection"
        );


    if (connected)
    {
        element.innerText =
            "● Connected";

        element.style.color =
            "#00ff66";
    }

    else
    {
        element.innerText =
            "● Connection lost";

        element.style.color =
            "#ff4444";
    }
}


// ============================================================
// UPDATE TOF DATA
// ============================================================

async function updateToF()
{
    try
    {
        const response =
            await fetch(
                "/tof",
                {
                    cache: "no-store"
                }
            );


        if (
            !response.ok
        )
        {
            throw new Error(
                "HTTP " +
                response.status
            );
        }


        const data =
            await response.json();


        setConnection(
            true
        );


        // ----------------------------------------------------
        // Update 64 cells
        // ----------------------------------------------------

        let validCount = 0;


        for (
            let i = 0;
            i < 64;
            i++
        )
        {
            const cell =
                document.getElementById(
                    "cell" + i
                );


            const distance =
                data.distance[i];


            const status =
                data.status[i];


            if (
                status === 5 &&
                distance > 0
            )
            {
                cell.innerText =
                    distance + " mm";


                cell.style.background =
                    depthColor(
                        distance
                    );


                validCount++;
            }

            else
            {
                cell.innerText =
                    "--";


                cell.style.background =
                    "#222222";
            }
        }


        // ----------------------------------------------------
        // Frame number
        // ----------------------------------------------------

        document.getElementById(
            "frame"
        ).innerText =
            data.sequence;


        // ----------------------------------------------------
        // Valid-zone count
        // ----------------------------------------------------

        document.getElementById(
            "valid"
        ).innerText =
            validCount +
            " / 64";
    }


    catch (error)
    {
        setConnection(
            false
        );

        console.log(
            "ToF error:",
            error
        );
    }
}


// ============================================================
// LIVE UPDATE
// ============================================================
//
// 100 ms = approximately 10 browser updates/sec.
//
// The VL53L5CX is configured for 15 Hz, so this gives us a
// responsive browser display without hammering the ESP32
// with unnecessary HTTP requests.
//

setInterval(
    updateToF,
    100
);


// Initial update

updateToF();


</script>


</body>

</html>

)rawliteral";


#endif
