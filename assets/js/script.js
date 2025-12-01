const DEFAULT_BASE_URL = (window.location.port !== '') ? ('http://' + window.location.host) : ('https://' + window.location.host)

let API_BASE_URL = DEFAULT_BASE_URL;
let API_ENDPOINTS = {
  fetch: '/v1/apps',
  install: '/v1/createToken',
  download: '/download'
};

let appsData = [];
let selectedApps = new Set();
let currentCategory = 'all';

const appContainer = document.getElementById('appContainer');
const selectedCountSpan = document.getElementById('selectedCount');
const installBtn = document.getElementById('installBtn');
const dynamicCategoriesList = document.getElementById('dynamicCategories');

function mapBackendCategory(backendCategory) {
    return backendCategory.trim().toLowerCase();
}

const lazyLoadIcons = () => {
    
    const lazyImages = document.querySelectorAll('img[data-src]');
    
    if ('loading' in HTMLImageElement.prototype) {
      lazyImages.forEach(img => {
            img.src = img.dataset.src;
            img.removeAttribute('data-src');
      })
        return; 
    }
        
    const observerOptions = {
      rootMargin: '0px 0px 50px 0px',
      threshold:0.01,
    };

    const imageObserver = new IntersectionObserver((entries, observer) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                const img = entry.target;
                img.src = img.dataset.src; 
                img.removeAttribute('data-src'); 
                observer.unobserve(img); 
            }
        });
    }, observerOptions);

    lazyImages.forEach(img => imageObserver.observe(img));
};

installBtn.onclick = handleInstall;

// async function fetchConfig() {
//     appContainer.innerHTML = `<div style="text-align: center; padding: 40px; font-size: 18px; color: #1d4ed8;">
//                                 <i class="fa-solid fa-cog fa-spin"></i> Loading Configuration...
//                               </div>`;

//     try {
//         const configURL = `${DEFAULT_BASE_URL}/v1/config`;
//         const response = await axios.get(configURL);

//         const config = response.data.api_config;

//         API_BASE_URL = config.baseUrl;
//         API_ENDPOINTS.fetch = config.listApps || config.fetch;
//         API_ENDPOINTS.install = config.getDownloadLinks || config.install;

//         await fetchData();

//     } catch (error) {
//         console.error("Failed to fetch configuration:", error);
//         appContainer.innerHTML = `<div style="text-align: center; padding: 40px; color: red;">
//                                     <i class="fa-solid fa-triangle-exclamation"></i> Error: Backend configuration (${DEFAULT_BASE_URL}/v1/config) not found.Please ensure the server is running
//                                   </div>`;
//     }
// }

async function fetchData() {

  try {

    const fetchURL = `${API_BASE_URL}${API_ENDPOINTS.fetch}`;
    const response = await axios.get(fetchURL);

    if (response.data.status && Array.isArray(response.data.result)) {

      appsData = response.data.result.map(app => ({
        id: app.id,
        name: app.app_name || app.name,
        category: mapBackendCategory(app.category),
        desc: app.description || 'No description available',

        icon_id: app.icon_id || app.id,
        color: app.color || '#333333'
      }));

      sidebarCounts();
      renderApps();
      lazyLoadIcons();

    } else {
      throw new Error('Backend returned status: false or invalid result format.');
    }

  } catch (error) {
    console.error("Failed to fetch app data from backend:", error);
    appContainer.innerHTML = `<div style="text-align: center; padding: 40px; color: red;">
                                    <i class="fa-solid fa-triangle-exclamation"></i> Error: Could not fetch data from ${API_BASE_URL}${API_ENDPOINTS.fetch}.
                                  </div>`;
  }
}

async function handleInstall() {
  if (selectedApps.size === 0) {
    alert("Please select at least one application to install.");
    return;
  }

  installBtn.innerHTML = `Downloading... <i class="fa-solid fa-circle-notch fa-spin"></i>`;
  installBtn.disabled = true;

  const installList = Array.from(selectedApps);

  try {
    const installURL = `${API_BASE_URL}${API_ENDPOINTS.install}`;
    console.log(installURL)

    const response = await axios.post(installURL, {
      appIds: installList
    });

    const result = response.data;

    if (result.status) {
      // Download the exe
      const downloadUrl = `${API_BASE_URL}${API_ENDPOINTS.download}/${result.tokenID}`
      await axios.get(downloadUrl, { responseType: 'blob' })
        .then((response) => {
          const url = window.URL.createObjectURL(new Blob([response.data]));
          const link = document.getElementById('downloadlink');
          link.href = url;
          link.click();
          window.URL.revokeObjectURL(url);
        })
        .catch((error) => {
          console.error('Download error:', error);
          alert(`Download failed! ${error.message}`);
        });


      selectedApps.clear();
      Footer();

      if (searchInput.value) {
        searchApps();
      } else {
        renderApps();
      }
    } else {
      alert(`Installation failed! Server error: ${result.message || 'Unknown error'}`);
    }

  } catch (error) {
    console.error("Error sending install request:", error);
    alert(`Network Error: Could not reach backend server.`);
  } finally {
    installBtn.disabled = false;
    Footer();
  }
}

function renderApps() {
  appContainer.innerHTML = '';

  const filteredApps = currentCategory === 'all'
    ? appsData
    : appsData.filter(app => app.category === currentCategory);

  if (filteredApps.length === 0) {
    appContainer.innerHTML = `<div style="text-align: center; padding: 40px; color: #64748b; grid-column: 1 / -1;">
                                    No apps found in the current category/search.
                                  </div>`;
    return;
  }

  filteredApps.forEach(app => {
    const isSelected = selectedApps.has(app.id);
    const card = document.createElement('div');
    card.className = `app-card ${isSelected ? 'selected' : ''}`;
    card.onclick = () => toggleSelection(app.id);

    card.innerHTML = `
    <div class="card-top">
        <div class="app-icon fallback-container" style="background-color: transparent;">
            <img 
                data-src="icons/${app.icon_id}.png" 
                alt="${app.name} Icon"
                class="lazy-icon"
                loading="lazy" 
                onerror="this.parentElement.style.backgroundColor='${app.color || '#cccccc'}';"
                src="data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7" 
            >
        </div>
        <div class="app-details"><h3>${app.name}</h3><p>${app.desc}</p></div>
    </div>
    <div class="card-bottom">
        <div class="select-text">${isSelected ? '<i class="fa-solid fa-check"></i> Selected (Click to Remove)' : 'Click to select'}</div>
        <button class="add-btn">${isSelected ? 'Added' : '+ Add'}</button>
    </div>  `;

    appContainer.appendChild(card);
  });
  lazyLoadIcons();
}

function toggleSelection(id) {
  if (selectedApps.has(id)) selectedApps.delete(id);
  else selectedApps.add(id);

  Footer();

  const searchInput = document.getElementById('searchInput');
  if (searchInput.value) {
    searchApps();
  } else {
    renderApps();
  }
}

function filterCategory(category, element) {
  searchInput.value = '';
  currentCategory = category;
  document.querySelectorAll('.sidebar li').forEach(li => li.classList.remove('active'));
  element.classList.add('active');
  renderApps();
}

function searchApps() {
  const query = searchInput.value.toLowerCase();

  const filteredApps = appsData.filter(app => app.name.toLowerCase().includes(query));

  appContainer.innerHTML = '';

  if (filteredApps.length === 0) {
    appContainer.innerHTML = `<div style="text-align: center; padding: 40px; color: #64748b; grid-column: 1 / -1;">
                                    No results found for "${query}".
                                  </div>`;
    return;
  }

  filteredApps.forEach(app => {
    const isSelected = selectedApps.has(app.id);
    const card = document.createElement('div');
    card.className = `app-card ${isSelected ? 'selected' : ''}`;
    card.onclick = () => toggleSelection(app.id);

    card.innerHTML = `
    <div class="card-top">
        <div class="app-icon fallback-container" style="background-color: transparent;">
           <img 
                data-src="icons/${app.icon_id}.png" 
                alt="${app.name} Icon"
                class="lazy-icon"
                loading="lazy"  
                onerror="this.parentElement.style.backgroundColor='${app.color || '#cccccc'}';"
                src="data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7" 
            >
        </div>
        <div class="app-details"><h3>${app.name}</h3><p>${app.desc}</p></div>
    </div>
    <div class="card-bottom">
        <div class="select-text">${isSelected ? '<i class="fa-solid fa-check"></i> Selected (Click to Remove)' : 'Click to select'}</div>
        <button class="add-btn">${isSelected ? 'Added' : '+ Add'}</button>
    </div>
`;
    appContainer.appendChild(card);

  });
  lazyLoadIcons();
}

function Footer() {
  const count = selectedApps.size;
  selectedCountSpan.innerText = count;

  installBtn.innerHTML = `INSTALL <i class="fa-solid fa-download"></i>`;

  if (count > 0) {
    installBtn.style.opacity = "1";
    installBtn.style.cursor = "pointer";
  } else {
    installBtn.style.opacity = "0.7";
    // installBtn.style.cursor="default";
  }
}

function sidebarCounts() {
    const categoryMap = {}; 
    appsData.forEach(app => {
        const cat = app.category;
        categoryMap[cat] = (categoryMap[cat] || 0) + 1;
    });

    renderSidebarCategories(categoryMap); 
    
    document.getElementById('count-all').innerText = appsData.length;
}

function renderSidebarCategories(categoryMap) {
    const sidebarList = document.getElementById('dynamicCategories'); 
    let html = '';

    sidebarList.innerHTML = ''; 

    const sortedCategories = Object.entries(categoryMap).sort(([nameA], [nameB]) => nameA.localeCompare(nameB));

    for (const [categoryName, count] of sortedCategories) {
        
        const displayCategory = categoryName.charAt(0).toUpperCase() + categoryName.slice(1);
        
        html += `
            <li onclick="filterCategory('${categoryName}', this)">
                <div class="category-name">${displayCategory}</div>
                <div class="category-count">${count}</div>
            </li>
        `;
    }

    sidebarList.innerHTML = html;
}

function openGuide() {
  document.getElementById('guideModal').style.display = 'flex';
}

function closeGuide() {
  document.getElementById('guideModal').style.display = 'none';
}

window.onclick = function (event) {
  const modal = document.getElementById('guideModal');
  if (event.target == modal) {
    modal.style.display = "none";
  }
}

// fetchConfig();

fetchData();