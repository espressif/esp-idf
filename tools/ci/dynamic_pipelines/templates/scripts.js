function debounce(func, wait) {
    let timeout;
    return function () {
        const context = this;
        const args = arguments;
        clearTimeout(timeout);
        timeout = setTimeout(() => func.apply(context, args), wait);
    };
}

function extractPipelineId() {
    return $("body").data("pipeline-id");
}

function updateTabsAvailability(jobs) {
    $("#build-report-tab, #job-report-tab, #test-report-tab").each(function () {
        const $tab = $(this);
        if (!$tab.hasClass("active")) {
            $tab.addClass("disabled");
            $tab.attr("title", "This report is not available yet");
            $tab.css("opacity", "0.5");
            $tab.css("cursor", "not-allowed");
            $tab.on("click", function (e) {
                if ($(this).hasClass("disabled")) {
                    e.preventDefault();
                    e.stopPropagation();
                    return false;
                }
            });
        }
    });

    const buildReportPatterns = [
        "generate_pytest_build_report",
        "built_apps",
        "build_report",
        "skipped_apps",
        "failed_apps",
    ];

    const jobReportPatterns = [
        "generate_failed_jobs_report",
        "job_report",
        "pipeline_jobs",
    ];

    const testReportPatterns = [
        "generate_pytest_report",
        "target_test_report",
        "test_report",
        "pytest_report",
    ];

    let buildReportJob = jobs.find((job) =>
        buildReportPatterns.some((pattern) => job.name.includes(pattern))
    );

    let jobReportJob = jobs.find((job) =>
        jobReportPatterns.some((pattern) => job.name.includes(pattern))
    );

    let testReportJob = jobs.find((job) =>
        testReportPatterns.some((pattern) => job.name.includes(pattern))
    );

    if (buildReportJob) reportJobIds.build = buildReportJob.job_id;
    if (jobReportJob) reportJobIds.job = jobReportJob.job_id;
    if (testReportJob) reportJobIds.test = testReportJob.job_id;

    const hasBuildReport = buildReportJob !== undefined;
    const hasJobReport = jobReportJob !== undefined;
    const hasTestReport = testReportJob !== undefined;

    if (hasBuildReport && !$("#build-report-tab").hasClass("active")) {
        $("#build-report-tab").removeClass("disabled");
        $("#build-report-tab").attr(
            "title",
            "View application build results including binary sizes and downloads"
        );
        $("#build-report-tab").css("opacity", "1");
        $("#build-report-tab").css("cursor", "pointer");
    }

    if (hasJobReport && !$("#job-report-tab").hasClass("active")) {
        $("#job-report-tab").removeClass("disabled");
        $("#job-report-tab").attr(
            "title",
            "View CI job results and failed jobs"
        );
        $("#job-report-tab").css("opacity", "1");
        $("#job-report-tab").css("cursor", "pointer");
    }

    if (hasTestReport && !$("#test-report-tab").hasClass("active")) {
        $("#test-report-tab").removeClass("disabled");
        $("#test-report-tab").attr(
            "title",
            "View test results including success, failure, and skipped tests"
        );
        $("#test-report-tab").css("opacity", "1");
        $("#test-report-tab").css("cursor", "pointer");
    }
}

$(document).ready(function () {
    const currentPath = window.location.pathname;
    const currentFile = currentPath.substring(currentPath.lastIndexOf("/") + 1);

    $(".report-nav-tabs .nav-tab").removeClass("active");

    if (currentFile.includes("built_apps")) {
        $("#build-report-tab").addClass("active");
    } else if (currentFile.includes("job_report")) {
        $("#job-report-tab").addClass("active");
    } else if (currentFile.includes("target_test_report")) {
        $("#test-report-tab").addClass("active");
    }

    wrapTablesInCollapsibleSections();
    makeTablesCollapsible();
    setupScrollProgressBar();
    setupFloatingActions();
    enhanceTableStatusDisplay();
    markTestCaseColumns();
    setupResponsiveText();
    setupTextToggles();
    setupEventHandlers();
    scrollToHashLocation();
    initBootstrapTable();
    fixStickyHeaderAlignment();
    forceEnableStickyHeaders();
    fixTableHeaderText();
    lazyLoadVisibleImages();
    setupPagination();
    optimizeScrollPerformance();
});

function fixStickyHeaderAlignment() {
    if (window.isScrolling) return;

    $(".sticky-header-container").each(function () {
        const $container = $(this);
        const $table = $container
            .closest(".bootstrap-table")
            .find(".fixed-table-body table");

        let tableWidth = "100%";
        if ($table.length) {
            const actualWidth = $table.width();
            if (actualWidth > 0) {
                tableWidth = actualWidth + "px";
            }
        }

        $container.css({
            left: "0",
            "margin-left": "0",
            "padding-left": "0",
            width: tableWidth,
            "z-index": "100",
            transform: "translateZ(0)",
        });

        this.style.setProperty("left", "0", "important");
        this.style.setProperty("margin-left", "0", "important");
        this.style.setProperty("padding-left", "0", "important");
        this.style.setProperty("width", tableWidth, "important");
        this.style.setProperty("z-index", "100", "important");
        this.style.setProperty("transform", "translateZ(0)", "important");

        const $headerTable = $container.find("table");
        if ($headerTable.length) {
            $headerTable.css("width", tableWidth);
            $headerTable[0].style.setProperty("width", tableWidth, "important");
        }

        if (!window.isScrolling) {
            if ($headerTable.length && $table.length) {
                const $headerCols = $headerTable.find("th");
                const $bodyCols = $table.find("tr:first-child td");

                if ($headerCols.length === $bodyCols.length) {
                    $headerCols.each(function (i) {
                        if (i < $bodyCols.length) {
                            const bodyColWidth = $($bodyCols[i]).outerWidth();
                            if (bodyColWidth > 0) {
                                $(this).css("min-width", bodyColWidth + "px");
                                $(this).css("width", bodyColWidth + "px");

                                const $thInner = $(this).find(".th-inner");
                                if ($thInner.length) {
                                    $thInner.css({
                                        "white-space": "normal",
                                        overflow: "visible",
                                        "text-overflow": "clip",
                                        height: "auto",
                                        display: "block",
                                    });
                                }
                            }
                        }
                    });
                }
            }
        }
    });

    $(".fixed-table-container").each(function () {
        if ($(this).css("left") === "20px") {
            $(this).css("left", "0");
            this.style.setProperty("left", "0", "important");
        }
    });

    $(".bootstrap-table .table thead th").css({
        position: "sticky",
        top: "0",
        "z-index": "100",
        "background-color": "var(--esp-light)",
        transform: "translateZ(0)",
        "white-space": "normal",
        overflow: "visible",
        "text-overflow": "clip",
        height: "auto",
    });

    $(
        ".bootstrap-table .table thead th .th-inner, .sticky-header-container th .th-inner"
    ).css({
        "white-space": "normal",
        overflow: "visible",
        "text-overflow": "clip",
        height: "auto",
        "min-height": "20px",
        display: "block",
        "line-height": "1.4",
    });
}

function setupScrollProgressBar() {
    const progressBar = $("#nav-progress-bar");
    let lastScrollPosition = 0;
    let ticking = false;

    $(window).on("scroll", function () {
        if (ticking) return;

        const scrollPosition = window.scrollY;

        if (Math.abs(scrollPosition - lastScrollPosition) < 5) {
            return;
        }

        lastScrollPosition = scrollPosition;

        window.requestAnimationFrame(function () {
            const windowHeight = $(document).height() - $(window).height();
            const scrollPercentage = (scrollPosition / windowHeight) * 100;
            progressBar.css("width", scrollPercentage + "%");
            ticking = false;
        });
        ticking = true;
    });
}

function setupFloatingActions() {
    $("#back-to-top").on("click", function () {
        $("html, body").animate({ scrollTop: 0 }, 300);
    });
}

function enhanceTableStatusDisplay() {
    $("table.table th").each(function (index) {
        const $header = $(this);
        const headerText = $header.text().trim().toLowerCase();

        if (
            headerText.includes("status") ||
            headerText.includes("result") ||
            headerText.includes("state")
        ) {
            const colIndex = index + 1;

            $(`table.table td:nth-child(${colIndex})`).each(function () {
                const $cell = $(this);
                const cellText = $cell.text().trim().toLowerCase();

                if (cellText.includes("pass") || cellText.includes("success")) {
                    $cell.html(
                        `<div class="status-badge success">${$cell.text()}</div>`
                    );
                } else if (
                    cellText.includes("fail") ||
                    cellText.includes("error")
                ) {
                    $cell.html(
                        `<div class="status-badge danger">${$cell.text()}</div>`
                    );
                } else if (
                    cellText.includes("warn") ||
                    cellText.includes("skip")
                ) {
                    $cell.html(
                        `<div class="status-badge warning">${$cell.text()}</div>`
                    );
                }
            });
        }
    });
}

function setupTextToggles() {
    setupResponsiveText();

    $(".toggle-link").off("click").on("click", toggleText);
}

function setupResponsiveText() {
    $("table.table td").each(function () {
        const $cell = $(this);
        const text = $cell.text();

        if (
            text.length > 100 &&
            !$cell.hasClass("test-case-name") &&
            !$cell.find(".text-toggle").length
        ) {
            const displayText = text.substring(0, 100) + "...";
            $cell.html(
                `<span class="text-toggle">${displayText}</span>` +
                    `<span class="full-text" style="display: none;">${text}</span>` +
                    `<a href="#" class="toggle-link"><i class="fas fa-plus-circle"></i> Show More</a>`
            );
        }
    });
}

function makeTablesCollapsible() {
    $("table.table").each(function (index) {
        const table = $(this);
        const tableId = `table-${index}`;
        table.attr("id", tableId);

        const bootstrapTableWrapper = table.closest(".bootstrap-table");

        let container = bootstrapTableWrapper.parent(".table-container");
        if (!container.length) {
            container = $('<div class="table-container"></div>');
        }

        let tableControls = bootstrapTableWrapper.prev(".table-controls");

        if (!tableControls.length) {
            const initialButtonHtml =
                '<i class="fas fa-minus"></i> <span>Collapse Table</span>';

            const toggleButton = $(`
                <button class="table-collapse-btn" data-table="${tableId}">
                    ${initialButtonHtml}
                </button>
            `);

            tableControls = $('<div class="table-controls"></div>').append(
                toggleButton
            );

            bootstrapTableWrapper.before(tableControls);
        } else {
            const existingButton = tableControls.find(".table-collapse-btn");
            if (existingButton.length) {
                existingButton.detach();
                tableControls.append(existingButton);
            }
        }

        if (!bootstrapTableWrapper.parent().hasClass("table-container")) {
            bootstrapTableWrapper.wrap(container);
        }

        bootstrapTableWrapper.show();
        bootstrapTableWrapper.addClass("expanded");

        const toggleButton = tableControls.find(".table-collapse-btn");
        toggleButton.off("click").on("click", function (e) {
            e.preventDefault();
            e.stopPropagation();

            const tableId = $(this).data("table");

            const targetTable = $(`#${tableId}`);
            if (!targetTable.length) {
                console.error("Target table not found:", tableId);
                return;
            }

            const bootstrapWrapper = targetTable.closest(".bootstrap-table");
            if (!bootstrapWrapper.length) {
                console.error(
                    "Bootstrap wrapper not found for table:",
                    tableId
                );
                return;
            }

            const isVisible = bootstrapWrapper.is(":visible");

            if (isVisible) {
                bootstrapWrapper.slideUp(300);
                bootstrapWrapper.removeClass("expanded");
                $(this).html(
                    '<i class="fas fa-plus"></i> <span>Expand Table</span>'
                );
            } else {
                bootstrapWrapper.slideDown(300);
                bootstrapWrapper.addClass("expanded");
                $(this).html(
                    '<i class="fas fa-minus"></i> <span>Collapse Table</span>'
                );
            }
        });
    });

    setTimeout(function () {
        $(".bootstrap-table[data-auto-collapse='true']").each(function () {
            const bootstrapWrapper = $(this);
            const tableId = bootstrapWrapper.find("table").attr("id");
            const toggleButton = $(`button[data-table="${tableId}"]`);

            bootstrapWrapper.slideUp(300);
            bootstrapWrapper.removeClass("expanded");

            if (toggleButton.length) {
                toggleButton.html(
                    '<i class="fas fa-plus"></i> <span>Expand Table</span>'
                );
            }
        });
    }, 500);
}

function expandAllTables() {
    $(".bootstrap-table").slideDown(300);
    $(".bootstrap-table").addClass("expanded");

    $(".table-collapse-btn").each(function () {
        $(this).html(
            '<i class="fas fa-minus"></i> <span>Collapse Table</span>'
        );
    });

    $(".section-header").attr("aria-expanded", "true");
    $(".section-body").slideDown(300);
}

function collapseAllTables() {
    $(".bootstrap-table").slideUp(300);
    $(".bootstrap-table").removeClass("expanded");

    $(".table-collapse-btn").each(function () {
        $(this).html('<i class="fas fa-plus"></i> <span>Expand Table</span>');
    });
}

function initBootstrapTable() {
    $("table.table").each(function () {
        const $table = $(this);

        $table.bootstrapTable({
            classes: "table table-bordered table-striped",
            height: 800,
            pagination: false,
            showColumns: true,
            showColumnsToggleAll: true,
            showToggle: false,
            clickToSelect: false,
            minimumCountColumns: 2,
            stickyHeader: true,
            stickyHeaderOffsetY: 0,
            theadClasses: "thead-light",
            toolbar: "#toolbar",
            resizable: true,
            checkOnInit: false,
            fixedColumns: true,
            fixedNumber: 0,
            sortStable: true,
            undefinedText: "-",
            showFullscreen: true,
            widthUnit: "%",
            headerStyle: function () {
                return {
                    css: {
                        "white-space": "normal",
                        overflow: "visible",
                        "text-overflow": "clip",
                        height: "auto",
                        "min-height": "50px",
                    },
                };
            },
        });

        $table.closest(".bootstrap-table").css({
            width: "100%",
            "max-width": "100%",
        });

        $table.closest(".fixed-table-body").css({
            "overflow-x": "auto",
        });

        $table.css({
            width: "100%",
            "min-width": "100%",
        });

        $table.find("thead th").each(function () {
            const $th = $(this);
            const $thInner = $th.find(".th-inner");

            if ($thInner.length) {
                $thInner.css({
                    "white-space": "normal",
                    overflow: "visible",
                    "text-overflow": "clip",
                    height: "auto",
                    "min-height": "20px",
                    display: "block",
                });
            }
        });
    });

    $(window).on(
        "resize",
        debounce(function () {
            $("table.table").bootstrapTable("resetView");
        }, 100)
    );
}

function wrapTablesInCollapsibleSections() {
    $("h2").each(function (index) {
        if ($(this).closest(".section-header").length > 0) {
            return;
        }

        const header = $(this);
        const headerText = header.text();
        const headerId = header.attr("id") || `section-header-${index}`;

        const nextTable = header.nextAll("table.table:first");

        if (nextTable.length) {
            if (nextTable.closest(".section-body").length > 0) {
                return;
            }

            const wrapper = $('<div class="section-collapsible"></div>');

            const sectionHeader = $(`
                <div class="section-header sticky-section-header" aria-expanded="true" data-target="#section-body-${index}">
                    <h2 id="${headerId}">
                        <span class="heading-text">${headerText}</span>
                        <i class="fas fa-link copy-link-icon" onclick="copyPermalink('#${headerId}')"></i>
                    </h2>
                    <span class="collapse-indicator"><i class="fas fa-chevron-down"></i></span>
                </div>
            `);

            const sectionBody = $(
                `<div id="section-body-${index}" class="section-body"></div>`
            );

            const bootstrapTable = nextTable.closest(".bootstrap-table");
            const tableContainer = bootstrapTable.closest(".table-container");

            if (tableContainer.length) {
                tableContainer.detach().appendTo(sectionBody);
            } else if (bootstrapTable.length) {
                bootstrapTable.detach().appendTo(sectionBody);
            } else {
                nextTable.detach().appendTo(sectionBody);
            }

            wrapper.append(sectionHeader).append(sectionBody);

            header.replaceWith(wrapper);
        }
    });

    $(".section-header")
        .off("click")
        .on("click", function (e) {
            if (
                $(e.target).hasClass("copy-link-icon") ||
                $(e.target).closest(".copy-link-icon").length ||
                $(e.target).hasClass("table-collapse-btn") ||
                $(e.target).closest(".table-collapse-btn").length
            ) {
                return;
            }

            const header = $(this);
            const targetId = header.data("target");
            const sectionBody = $(targetId);
            const isExpanded = header.attr("aria-expanded") === "true";

            if (isExpanded) {
                header.attr("aria-expanded", "false");
                sectionBody.slideUp(300);
            } else {
                header.attr("aria-expanded", "true");
                sectionBody.slideDown(300);
            }
        });
}

function markTestCaseColumns() {
    $("table.table th").each(function (index) {
        const headerText = $(this).text().trim().toLowerCase();
        if (
            headerText.includes("test") &&
            (headerText.includes("case") || headerText.includes("name"))
        ) {
            const colIndex = index + 1;
            $(`table.table td:nth-child(${colIndex})`).addClass(
                "test-case-name"
            );
        }
    });
}

function setupEventHandlers() {
    $(window).on("load", scrollToHashLocation);
    $("body").on("click", ".toggle-link", toggleText);

    $("#expand-all-tables")
        .off("click")
        .on("click", function (e) {
            e.preventDefault();
            expandAllTables();
        });

    $("#collapse-all-tables")
        .off("click")
        .on("click", function (e) {
            e.preventDefault();
            collapseAllTables();
        });

    $("#pagination-size").on("change", function () {
        const pageSize = parseInt($(this).val(), 10);
        applyPaginationToTables(pageSize);
    });

    $("#show-all-columns").on("click", function (e) {
        e.preventDefault();
        $("table.table").bootstrapTable("showAllColumns");
    });

    $("#show-failed-only").on("click", function (e) {
        e.preventDefault();
        filterByStatus("fail");
    });

    $("#show-passed-only").on("click", function (e) {
        e.preventDefault();
        filterByStatus("pass");
    });

    $("#export-csv").on("click", function (e) {
        e.preventDefault();
        $("table.table:visible").bootstrapTable("exportTable", {
            type: "csv",
            fileName: "report_export_" + new Date().toISOString().slice(0, 10),
        });
    });

    $("#clear-all-filters").on("click", function () {
        clearAllFilters();
    });
}

function filterByStatus(status) {
    $("#active-filters").show();
    $("#filter-badges").html(
        `<span class="badge bg-primary me-2">Status: ${status}</span>`
    );

    $("table.table").each(function () {
        const $table = $(this);

        let statusColIndex = -1;
        $table.find("th").each(function (index) {
            const headerText = $(this).text().trim().toLowerCase();
            if (
                headerText.includes("status") ||
                headerText.includes("result") ||
                headerText.includes("state")
            ) {
                statusColIndex = index;
                return false;
            }
        });

        if (statusColIndex >= 0) {
            $table.bootstrapTable(
                "filterBy",
                {
                    [statusColIndex]: status,
                },
                {
                    filterAlgorithm: function (row, filters) {
                        const cellText = $(row[statusColIndex])
                            .text()
                            .trim()
                            .toLowerCase();
                        return cellText.includes(status);
                    },
                }
            );
        }
    });
}

function clearAllFilters() {
    $("table.table").each(function () {
        const $table = $(this);
        $table.bootstrapTable("clearFilterControl");
        $table.bootstrapTable("resetSearch");
    });

    $("#active-filters").hide();
    $("#filter-badges").empty();
}

function scrollToHashLocation() {
    const hash = window.location.hash;
    if (hash) {
        setTimeout(() => {
            const target = $(hash);
            if (target.length) {
                const sectionBody = target.closest(".section-body");
                if (sectionBody.length) {
                    const sectionHeader = sectionBody.prev(".section-header");
                    if (sectionHeader.attr("aria-expanded") !== "true") {
                        sectionHeader.attr("aria-expanded", "true");
                        sectionBody.slideDown(0);
                    }

                    const containingTable = target.closest("table.table");
                    if (containingTable.length) {
                        const bootstrapWrapper =
                            containingTable.closest(".bootstrap-table");
                        if (!bootstrapWrapper.is(":visible")) {
                            bootstrapWrapper.show();
                            const tableBtn = bootstrapWrapper
                                .parent()
                                .find(".table-controls")
                                .find(".table-collapse-btn");
                            tableBtn.html(
                                '<i class="fas fa-minus"></i> <span>Collapse Table</span>'
                            );
                        }
                    }
                }

                $("html, body").animate(
                    { scrollTop: target.offset().top - 20 },
                    100
                );
            }
        }, 300);
    }
}

function copyPermalink(anchorId) {
    const fullUrl = `${window.location.origin}${window.location.pathname}${anchorId}`;
    history.pushState(null, null, anchorId);
    navigator.clipboard.writeText(fullUrl);

    const tooltip = $(`
        <div class="alert alert-success position-fixed"
             style="top: 20px; right: 20px; z-index: 9999; animation: fadeIn 0.3s, fadeOut 0.3s 1.7s forwards;">
            <i class="fas fa-check-circle me-2"></i>Link copied to clipboard!
        </div>
    `);

    const icon = $(event.target).closest(".copy-link-icon");
    if (icon.length) {
        const originalClass = icon.attr("class");

        icon.removeClass()
            .addClass("fas fa-check copy-link-icon")
            .css("color", "var(--esp-success)");

        setTimeout(() => {
            icon.attr("class", originalClass).css("color", "");
        }, 1500);
    }

    $("body").append(tooltip);
    setTimeout(() => tooltip.remove(), 2000);
    scrollToHashLocation();
}

function toggleText(e) {
    e.preventDefault();
    e.stopPropagation();

    const link = $(this);
    const textSpan = link.siblings(".full-text");
    const toggleSpan = link.siblings(".text-toggle");

    const visible = textSpan.is(":visible");

    if (visible) {
        link.html('<i class="fas fa-plus-circle"></i> Show More');
        textSpan.hide();
        toggleSpan.show();
    } else {
        link.html('<i class="fas fa-minus-circle"></i> Show Less');
        textSpan.show();
        toggleSpan.hide();
    }
}

function setupPagination() {
    applyPaginationToTables(25);
}

function applyPaginationToTables(pageSize) {
    localStorage.setItem("paginationSize", pageSize);

    $("table.table").each(function () {
        const $table = $(this);

        if (pageSize > 0) {
            $table.bootstrapTable("refreshOptions", {
                pagination: true,
                pageSize: pageSize,
                pageList: [10, 25, 50, "All"],
            });

            $table
                .closest(".bootstrap-table")
                .find(".fixed-table-pagination")
                .css({
                    display: "block",
                    visibility: "visible",
                    "background-color": "var(--esp-light)",
                    padding: "10px",
                    "border-top": "1px solid rgba(0, 0, 0, 0.05)",
                });
        } else {
            $table.bootstrapTable("refreshOptions", {
                pagination: false,
            });

            $table
                .closest(".bootstrap-table")
                .find(".fixed-table-pagination")
                .css({
                    display: "none",
                    visibility: "hidden",
                });
        }
    });
}

function optimizeScrollPerformance() {
    window.isScrolling = false;
    let scrollTimer = null;
    let lastScrollTop = 0;

    cacheAndFixDomElements();

    const progressBar = document.getElementById("nav-progress-bar");
    const backToTop = document.getElementById("back-to-top");

    window.addEventListener(
        "scroll",
        function () {
            window.isScrolling = true;

            const scrollTop = window.scrollY;

            if (Math.abs(scrollTop - lastScrollTop) > 5) {
                lastScrollTop = scrollTop;

                handleEssentialScrollUpdates(scrollTop, progressBar, backToTop);
            }

            clearTimeout(scrollTimer);
            scrollTimer = setTimeout(function () {
                window.isScrolling = false;
                requestAnimationFrame(function () {
                    fixStickyHeaderAlignment();
                    fixTableHeaderText();
                });
            }, 150);
        },
        { passive: true }
    );

    lazyLoadVisibleImages();
}

function handleEssentialScrollUpdates(scrollTop, progressBar, backToTop) {
    requestAnimationFrame(function () {
        const docHeight =
            Math.max(
                document.body.scrollHeight,
                document.body.offsetHeight,
                document.documentElement.clientHeight,
                document.documentElement.scrollHeight,
                document.documentElement.offsetHeight
            ) - window.innerHeight;
        const scrollPercentage = (scrollTop / docHeight) * 100;
        progressBar.style.width = scrollPercentage + "%";

        if (scrollTop > 200) {
            backToTop.classList.add("visible");
        } else {
            backToTop.classList.remove("visible");
        }
    });
}

function cacheAndFixDomElements() {
    const stickyElements = document.querySelectorAll(
        ".sticky-header-container, .section-header, .bootstrap-table thead, .fixed-table-header"
    );

    for (let i = 0; i < stickyElements.length; i++) {
        const elem = stickyElements[i];
        elem.style.transform = "translateZ(0)";
        elem.style.willChange = "transform";
        elem.style.backfaceVisibility = "hidden";
    }

    document.body.style.willChange = "scroll-position";
    document.body.style.backfaceVisibility = "hidden";

    const headers = document.querySelectorAll(
        ".bootstrap-table .table thead th, .sticky-header-container th"
    );
    for (let i = 0; i < headers.length; i++) {
        const header = headers[i];
        header.style.position = "sticky";
        header.style.top = "0";
        header.style.zIndex = "100";
        header.style.transform = "translateZ(0)";

        const thInners = header.querySelectorAll(".th-inner");
        for (let j = 0; j < thInners.length; j++) {
            const inner = thInners[j];
            inner.style.whiteSpace = "normal";
            inner.style.overflow = "visible";
            inner.style.textOverflow = "clip";
            inner.style.height = "auto";
            inner.style.minHeight = "20px";
            inner.style.display = "block";
        }
    }
}

function lazyLoadVisibleImages() {
    const lazyImages = document.querySelectorAll("img[data-src]");
    if (lazyImages.length === 0) return;

    const loadImage = function (img) {
        if (img.dataset.src) {
            img.src = img.dataset.src;
            img.removeAttribute("data-src");
        }
    };

    lazyImages.forEach((img) => {
        if (isElementInViewport(img)) {
            loadImage(img);
        }
    });

    document.addEventListener(
        "scroll",
        debounce(function () {
            lazyImages.forEach((img) => {
                if (img.dataset.src && isElementInViewport(img)) {
                    loadImage(img);
                }
            });
        }, 200),
        { passive: true }
    );
}

function isElementInViewport(el) {
    const rect = el.getBoundingClientRect();
    return (
        rect.top >= 0 &&
        rect.left >= 0 &&
        rect.bottom <=
            (window.innerHeight || document.documentElement.clientHeight) &&
        rect.right <=
            (window.innerWidth || document.documentElement.clientWidth)
    );
}

function fixTableHeaderText() {
    if (window.isScrolling) return;

    $(".bootstrap-table .table thead th, .sticky-header-container th").each(
        function () {
            const $th = $(this);

            $th.css({
                "white-space": "normal",
                overflow: "visible",
                "text-overflow": "clip",
                height: "auto",
                "min-height": "50px",
            });

            const $thInner = $th.find(".th-inner");
            if ($thInner.length) {
                $thInner.css({
                    "white-space": "normal",
                    overflow: "visible",
                    "text-overflow": "clip",
                    height: "auto",
                    "min-height": "20px",
                    display: "block",
                    "line-height": "1.4",
                });

                $thInner.find("span, div").css({
                    "white-space": "normal",
                    overflow: "visible",
                });
            }
        }
    );
}

function forceEnableStickyHeaders() {
    if (!window.isScrolling) {
        $("table.table thead th").css({
            position: "sticky",
            top: "0",
            "z-index": "100",
            "background-color": "var(--esp-light)",
            transform: "translateZ(0)",
            "will-change": "transform",
            "backface-visibility": "hidden",
            "white-space": "normal",
            overflow: "visible",
            "text-overflow": "clip",
            height: "auto",
            "min-height": "50px",
        });

        $("table.table").each(function () {
            const $table = $(this);
            const bootstrapTable = $table.closest(".bootstrap-table");

            if (
                bootstrapTable.length &&
                bootstrapTable.find(".sticky-header-container").length === 0
            ) {
                const $thead = $table.find("thead").clone();
                const $stickyContainer = $(
                    '<div class="sticky-header-container"></div>'
                );
                const $stickyTable = $('<table class="table"></table>').append(
                    $thead
                );

                $stickyContainer.append($stickyTable);
                bootstrapTable.prepend($stickyContainer);

                const tableWidth = $table.width();
                $stickyContainer.css({
                    position: "sticky",
                    top: "0",
                    "z-index": "100",
                    width: tableWidth + "px",
                    overflow: "hidden",
                    "background-color": "var(--esp-light)",
                    "will-change": "transform",
                    "backface-visibility": "hidden",
                    transform: "translateZ(0)",
                });

                const $originalThs = $table.find("thead th");
                const $stickyThs = $stickyTable.find("th");

                $originalThs.each(function (i) {
                    if (i < $stickyThs.length) {
                        const width = $(this).outerWidth();
                        $($stickyThs[i]).css({
                            width: width + "px",
                            "white-space": "normal",
                            overflow: "visible",
                            "text-overflow": "clip",
                            height: "auto",
                            "min-height": "50px",
                        });

                        const $inner = $($stickyThs[i]).find(".th-inner");
                        if ($inner.length) {
                            $inner.css({
                                "white-space": "normal",
                                overflow: "visible",
                                "text-overflow": "clip",
                                height: "auto",
                                "min-height": "20px",
                                display: "block",
                                "line-height": "1.4",
                            });
                        }
                    }
                });
            }
        });
    }
}
